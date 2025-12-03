#include "vulkan/material_factory.h"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <format>
#include <optional>
#include <tiny_gltf.h>

namespace
{
using namespace steeplejack;
namespace fs = std::filesystem;

constexpr auto kDefaultNormal = "__default.normal.flat";
constexpr auto kDefaultMr = "__default.mr.neutral";
constexpr auto kDefaultEmissive = "__default.emissive.black";
constexpr auto kDefaultBaseColor = "__default.baseColor.white";

constexpr unsigned char kAlphaOpaque = 255;
constexpr unsigned char kChannelMax = 255;
constexpr unsigned char kChannelMin = 0;

constexpr unsigned char kNormalR = 128;
constexpr unsigned char kNormalG = 128;
constexpr unsigned char kNormalB = 255;

constexpr unsigned char kMetallicMax = 255;
constexpr unsigned char kRoughnessMax = 255;
constexpr unsigned char kMetallicZero = 0;

tinygltf::Model load_gltf_model(const fs::path& full_path)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    const auto ext = full_path.extension().string();
    bool loaded = false;
    if (ext == ".glb" || ext == ".GLB")
    {
        loaded = loader.LoadBinaryFromFile(&model, &err, &warn, full_path.string());
    }
    else
    {
        loaded = loader.LoadASCIIFromFile(&model, &err, &warn, full_path.string());
    }

    if (!warn.empty())
    {
        spdlog::warn("tinygltf warn {}: {}", full_path.string(), warn);
    }
    if (!loaded)
    {
        throw std::runtime_error(std::format("Failed to load glTF {}: {}", full_path.string(), err));
    }

    return model;
}

const tinygltf::Material& get_gltf_material(const tinygltf::Model& model, int material_index, const fs::path& full_path)
{
    if (material_index < 0 || static_cast<size_t>(material_index) >= model.materials.size())
    {
        throw std::runtime_error(
            std::format("Material index {} out of range for {}", material_index, full_path.string()));
    }
    return model.materials.at(static_cast<size_t>(material_index));
}

template <typename TexInfo>
std::optional<std::string> resolve_rel_image(
    const tinygltf::Model& model, const TexInfo& tex_info, const std::string& gltf_relpath, const fs::path& full_path)
{
    if (tex_info.index < 0 || static_cast<size_t>(tex_info.index) >= model.textures.size())
    {
        return std::nullopt;
    }
    const int image_index = model.textures.at(static_cast<size_t>(tex_info.index)).source;
    if (image_index < 0 || static_cast<size_t>(image_index) >= model.images.size())
    {
        return std::nullopt;
    }
    const auto& image = model.images.at(static_cast<size_t>(image_index));
    if (image.uri.empty())
    {
        throw std::runtime_error(std::format("Embedded image not supported in first pass for {}", full_path.string()));
    }
    const fs::path rel_dir = fs::path(gltf_relpath).parent_path();
    return (rel_dir / image.uri).generic_string();
}

} // namespace

using namespace steeplejack;

MaterialFactory::MaterialFactory(const Device& device, TextureFactory& textures) :
    m_device(device), m_textures(textures)
{
}

void MaterialFactory::clear()
{
    m_materials.clear();
}

void MaterialFactory::remove(const std::string& name)
{
    m_materials.erase(name);
}

Material& MaterialFactory::load_gltf(const std::string& name, const std::string& gltf_relpath, int material_index)
{
    namespace fs = std::filesystem;

    if (auto it = m_materials.find(name); it != m_materials.end())
    {
        Material& existing = *it->second;
        if (existing.gltf_relpath() == gltf_relpath && existing.material_index() == material_index)
        {
            return existing;
        }

        throw std::runtime_error(std::format(
            "Material {} already loaded from {} index {}, requested {} index {}",
            name,
            existing.gltf_relpath(),
            existing.material_index(),
            gltf_relpath,
            material_index));
    }

    m_textures.ensure_fallback(
        kDefaultBaseColor,
        kChannelMax,
        kChannelMax,
        kChannelMax,
        kAlphaOpaque,
        TextureColorSpace::Srgb);

    m_textures.ensure_fallback(kDefaultNormal, kNormalR, kNormalG, kNormalB, kAlphaOpaque, TextureColorSpace::Linear);

    m_textures.ensure_fallback(
        kDefaultMr,
        kMetallicMax,
        kRoughnessMax,
        kMetallicZero,
        kAlphaOpaque,
        TextureColorSpace::Linear);

    m_textures.ensure_fallback(
        kDefaultEmissive,
        kChannelMin,
        kChannelMin,
        kChannelMin,
        kAlphaOpaque,
        TextureColorSpace::Srgb);

    const fs::path full_path = fs::path("assets/textures") / gltf_relpath;

    const tinygltf::Model model = load_gltf_model(full_path);
    const auto& mat = get_gltf_material(model, material_index, full_path);

    auto material = std::make_unique<Material>(m_device);
    material->m_name = name;
    material->m_gltf_relpath = gltf_relpath;
    material->m_material_index = material_index;

    static const std::unordered_map<std::string, AlphaMode> alpha_modes{
        {"MASK", AlphaMode::Mask},
        {"BLEND", AlphaMode::Blend},
    };
    const auto alpha_it = alpha_modes.find(mat.alphaMode);
    material->set_alpha_mode(alpha_it != alpha_modes.end() ? alpha_it->second : AlphaMode::Opaque);
    material->set_double_sided(mat.doubleSided);
    material->set_alpha_cutoff(static_cast<float>(mat.alphaCutoff));

    if (mat.pbrMetallicRoughness.baseColorFactor.size() == 4)
    {
        material->set_base_color_factor(
            glm::vec4(
                static_cast<float>(mat.pbrMetallicRoughness.baseColorFactor.at(0)),
                static_cast<float>(mat.pbrMetallicRoughness.baseColorFactor.at(1)),
                static_cast<float>(mat.pbrMetallicRoughness.baseColorFactor.at(2)),
                static_cast<float>(mat.pbrMetallicRoughness.baseColorFactor.at(3))));
    }
    material->set_metallic_factor(static_cast<float>(mat.pbrMetallicRoughness.metallicFactor));
    material->set_roughness_factor(static_cast<float>(mat.pbrMetallicRoughness.roughnessFactor));
    material->set_normal_scale(static_cast<float>(mat.normalTexture.scale));

    auto load_texture = [&](const auto& tex_info,
                            const std::string& suffix,
                            TextureColorSpace color_space,
                            const std::string& default_name,
                            Texture* (Material::*getter)() const,
                            void (Material::*setter)(Texture*))
    {
        auto rel_image =
            resolve_rel_image(model, tex_info, gltf_relpath, std::filesystem::path("assets/textures") / gltf_relpath);

        if (rel_image)
        {
            const auto tex_name = name + "." + suffix;
            if (color_space == TextureColorSpace::Srgb)
            {
                spdlog::info("Material {} {} -> {}", name, suffix, *rel_image);
            }
            m_textures.load(tex_name, *rel_image, color_space);
            (material.get()->*setter)(m_textures.at(tex_name));
        }

        if ((material.get()->*getter)() == nullptr)
        {
            (material.get()->*setter)(m_textures.at(default_name));
        }
    };

    load_texture(
        mat.pbrMetallicRoughness.baseColorTexture,
        "baseColor",
        TextureColorSpace::Srgb,
        kDefaultBaseColor,
        &Material::base_color,
        &Material::set_base_color);

    load_texture(
        mat.normalTexture,
        "normal",
        TextureColorSpace::Linear,
        kDefaultNormal,
        &Material::normal,
        &Material::set_normal);

    load_texture(
        mat.pbrMetallicRoughness.metallicRoughnessTexture,
        "metallicRoughness",
        TextureColorSpace::Linear,
        kDefaultMr,
        &Material::metallic_roughness,
        &Material::set_metallic_roughness);

    load_texture(
        mat.emissiveTexture,
        "emissive",
        TextureColorSpace::Srgb,
        kDefaultEmissive,
        &Material::emissive,
        &Material::set_emissive);

    auto& ref = *material;
    m_materials[name] = std::move(material);
    return ref;
}

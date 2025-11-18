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

void apply_pbr_factors(Material& target, const tinygltf::Material& source)
{
    if (source.pbrMetallicRoughness.baseColorFactor.size() == 4)
    {
        target.base_color_factor() = glm::vec4(
            static_cast<float>(source.pbrMetallicRoughness.baseColorFactor.at(0)),
            static_cast<float>(source.pbrMetallicRoughness.baseColorFactor.at(1)),
            static_cast<float>(source.pbrMetallicRoughness.baseColorFactor.at(2)),
            static_cast<float>(source.pbrMetallicRoughness.baseColorFactor.at(3)));
    }
    target.alpha_cutoff() = static_cast<float>(source.alphaCutoff);
}

void apply_alpha_mode(Material& target, const tinygltf::Material& source)
{
    target.set_double_sided(source.doubleSided);
    if (source.alphaMode == "MASK")
    {
        target.set_alpha_mode(AlphaMode::Mask);
    }
    else if (source.alphaMode == "BLEND")
    {
        target.set_alpha_mode(AlphaMode::Blend);
    }
    else
    {
        target.set_alpha_mode(AlphaMode::Opaque);
    }
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

Material* MaterialFactory::operator[](const std::string& name)
{
    auto it = m_materials.find(name);
    if (it == m_materials.end())
    {
        throw std::runtime_error(std::format("Material {} not found", name));
    }
    return it->second.get();
}

Material& MaterialFactory::create_unlit(const std::string& name, const std::string& texture_relpath)
{
    auto tex_name = name + ".baseColor";
    m_textures.load_texture(tex_name, texture_relpath, TextureColorSpace::Srgb);

    auto material = std::make_unique<Material>(m_device);
    material->set_base_color(m_textures[tex_name]);

    auto& ref = *material;
    m_materials[name] = std::move(material);

    // Defaults for other maps
    m_textures
        .ensure_texture_rgba_1x1(kDefaultNormal, kNormalR, kNormalG, kNormalB, kAlphaOpaque, TextureColorSpace::Linear);
    m_textures.ensure_texture_rgba_1x1(
        kDefaultMr,
        kMetallicMax,
        kRoughnessMax,
        kMetallicZero,
        kAlphaOpaque,
        TextureColorSpace::Linear);
    m_textures.ensure_texture_rgba_1x1(kDefaultEmissive, 0, 0, 0, kAlphaOpaque, TextureColorSpace::Srgb);
    ref.set_normal(m_textures[kDefaultNormal]);
    ref.set_metallic_roughness(m_textures[kDefaultMr]);
    ref.set_emissive(m_textures[kDefaultEmissive]);
    return ref;
}

template <typename TexInfo, typename Setter>
void MaterialFactory::load_texture_if_present(
    const tinygltf::Model& model,
    const std::string& name,
    const std::string& gltf_relpath,
    const TexInfo& tex_info,
    const std::string& suffix,
    TextureColorSpace color_space,
    Setter&& setter) const
{
    auto rel_image =
        resolve_rel_image(model, tex_info, gltf_relpath, std::filesystem::path("assets/textures") / gltf_relpath);
    if (!rel_image)
    {
        return;
    }
    const auto tex_name = name + "." + suffix;
    if (color_space == TextureColorSpace::Srgb)
    {
        spdlog::info("Material {} {} -> {}", name, suffix, *rel_image);
    }
    m_textures.load_texture(tex_name, *rel_image, color_space);
    std::forward<Setter>(setter)(m_textures[tex_name]);
}

void MaterialFactory::apply_defaults(Material& target)
{
    m_textures.ensure_texture_rgba_1x1(
        kDefaultBaseColor,
        kChannelMax,
        kChannelMax,
        kChannelMax,
        kAlphaOpaque,
        TextureColorSpace::Srgb);
    if (target.base_color() == nullptr)
    {
        target.set_base_color(m_textures[kDefaultBaseColor]);
    }

    m_textures
        .ensure_texture_rgba_1x1(kDefaultNormal, kNormalR, kNormalG, kNormalB, kAlphaOpaque, TextureColorSpace::Linear);
    if (target.normal() == nullptr)
    {
        target.set_normal(m_textures[kDefaultNormal]);
    }

    m_textures.ensure_texture_rgba_1x1(
        kDefaultMr,
        kMetallicMax,
        kRoughnessMax,
        kMetallicZero,
        kAlphaOpaque,
        TextureColorSpace::Linear);
    if (target.metallic_roughness() == nullptr)
    {
        target.set_metallic_roughness(m_textures[kDefaultMr]);
    }

    m_textures.ensure_texture_rgba_1x1(
        kDefaultEmissive,
        kChannelMin,
        kChannelMin,
        kChannelMin,
        kAlphaOpaque,
        TextureColorSpace::Srgb);
    if (target.emissive() == nullptr)
    {
        target.set_emissive(m_textures[kDefaultEmissive]);
    }
}

Material&
MaterialFactory::load_gltf_material(const std::string& name, const std::string& gltf_relpath, int material_index)
{
    namespace fs = std::filesystem;

    const fs::path full_path = fs::path("assets/textures") / gltf_relpath;

    const tinygltf::Model model = load_gltf_model(full_path);
    const auto& mat = get_gltf_material(model, material_index, full_path);

    auto material = std::make_unique<Material>(m_device);

    apply_pbr_factors(*material, mat);
    apply_alpha_mode(*material, mat);

    load_texture_if_present(
        model,
        name,
        gltf_relpath,
        mat.pbrMetallicRoughness.baseColorTexture,
        "baseColor",
        TextureColorSpace::Srgb,
        [&](Texture* tex) { material->set_base_color(tex); });
    load_texture_if_present(
        model,
        name,
        gltf_relpath,
        mat.normalTexture,
        "normal",
        TextureColorSpace::Linear,
        [&](Texture* tex) { material->set_normal(tex); });
    load_texture_if_present(
        model,
        name,
        gltf_relpath,
        mat.pbrMetallicRoughness.metallicRoughnessTexture,
        "metallicRoughness",
        TextureColorSpace::Linear,
        [&](Texture* tex) { material->set_metallic_roughness(tex); });
    load_texture_if_present(
        model,
        name,
        gltf_relpath,
        mat.emissiveTexture,
        "emissive",
        TextureColorSpace::Srgb,
        [&](Texture* tex) { material->set_emissive(tex); });

    apply_defaults(*material);

    auto& ref = *material;
    m_materials[name] = std::move(material);
    return ref;
}

#include "vulkan/material_factory.h"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <format>
#include <tiny_gltf.h>

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
    m_textures.ensure_texture_rgba_1x1("__default.normal.flat", 128, 128, 255, 255, TextureColorSpace::Linear);
    m_textures.ensure_texture_rgba_1x1("__default.mr.neutral", 255, 255, 0, 255, TextureColorSpace::Linear);
    m_textures.ensure_texture_rgba_1x1("__default.emissive.black", 0, 0, 0, 255, TextureColorSpace::Srgb);
    ref.set_normal(m_textures["__default.normal.flat"]);
    ref.set_metallic_roughness(m_textures["__default.mr.neutral"]);
    ref.set_emissive(m_textures["__default.emissive.black"]);
    return ref;
}

Material&
MaterialFactory::load_gltf_material(const std::string& name, const std::string& gltf_relpath, int material_index)
{
    namespace fs = std::filesystem;

    const fs::path full_path = fs::path("assets/textures") / gltf_relpath;

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

    if (material_index < 0 || static_cast<size_t>(material_index) >= model.materials.size())
    {
        throw std::runtime_error(
            std::format("Material index {} out of range for {}", material_index, full_path.string()));
    }

    const auto& mat = model.materials[static_cast<size_t>(material_index)];

    auto material = std::make_unique<Material>(m_device);

    // Base color factors
    if (mat.pbrMetallicRoughness.baseColorFactor.size() == 4)
    {
        auto& f = material->base_color_factor();
        f.r = static_cast<float>(mat.pbrMetallicRoughness.baseColorFactor[0]);
        f.g = static_cast<float>(mat.pbrMetallicRoughness.baseColorFactor[1]);
        f.b = static_cast<float>(mat.pbrMetallicRoughness.baseColorFactor[2]);
        f.a = static_cast<float>(mat.pbrMetallicRoughness.baseColorFactor[3]);
    }

    material->set_double_sided(mat.doubleSided);
    if (mat.alphaMode == "MASK")
    {
        material->set_alpha_mode(AlphaMode::Mask);
    }
    else if (mat.alphaMode == "BLEND")
    {
        material->set_alpha_mode(AlphaMode::Blend);
    }
    else
    {
        material->set_alpha_mode(AlphaMode::Opaque);
    }
    material->alpha_cutoff() = static_cast<float>(mat.alphaCutoff);

    // Base color texture if present
    const auto tex_index = mat.pbrMetallicRoughness.baseColorTexture.index;
    if (tex_index >= 0 && static_cast<size_t>(tex_index) < model.textures.size())
    {
        const int img_index = model.textures[static_cast<size_t>(tex_index)].source;
        if (img_index >= 0 && static_cast<size_t>(img_index) < model.images.size())
        {
            const auto& image = model.images[static_cast<size_t>(img_index)];
            if (!image.uri.empty())
            {
                const fs::path rel_dir = fs::path(gltf_relpath).parent_path();
                const auto rel_image = (rel_dir / image.uri).generic_string();
                auto tex_name = name + ".baseColor";
                spdlog::info("Material {} baseColor -> {}", name, rel_image);
                m_textures.load_texture(tex_name, rel_image, TextureColorSpace::Srgb);
                material->set_base_color(m_textures[tex_name]);
            }
            else
            {
                throw std::runtime_error(
                    std::format("Embedded baseColor image not supported in first pass for {}", full_path.string()));
            }
        }
    }

    // Normal texture (linear)
    if (mat.normalTexture.index >= 0 && static_cast<size_t>(mat.normalTexture.index) < model.textures.size())
    {
        const int img_index_n = model.textures[static_cast<size_t>(mat.normalTexture.index)].source;
        if (img_index_n >= 0 && static_cast<size_t>(img_index_n) < model.images.size())
        {
            const auto& img = model.images[static_cast<size_t>(img_index_n)];
            if (!img.uri.empty())
            {
                const fs::path rel_dir = fs::path(gltf_relpath).parent_path();
                const auto rel = (rel_dir / img.uri).generic_string();
                auto tex_name = name + ".normal";
                m_textures.load_texture(tex_name, rel, TextureColorSpace::Linear);
                material->set_normal(m_textures[tex_name]);
            }
        }
    }

    // Metallic-Roughness (linear). Some assets pack ORM; we treat as MR for now.
    if (mat.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0 &&
        static_cast<size_t>(mat.pbrMetallicRoughness.metallicRoughnessTexture.index) < model.textures.size())
    {
        const int img_index_mr =
            model.textures[static_cast<size_t>(mat.pbrMetallicRoughness.metallicRoughnessTexture.index)].source;
        if (img_index_mr >= 0 && static_cast<size_t>(img_index_mr) < model.images.size())
        {
            const auto& img = model.images[static_cast<size_t>(img_index_mr)];
            if (!img.uri.empty())
            {
                const fs::path rel_dir = fs::path(gltf_relpath).parent_path();
                const auto rel = (rel_dir / img.uri).generic_string();
                auto tex_name = name + ".metallicRoughness";
                m_textures.load_texture(tex_name, rel, TextureColorSpace::Linear);
                material->set_metallic_roughness(m_textures[tex_name]);
            }
        }
    }

    // Emissive texture (sRGB)
    if (mat.emissiveTexture.index >= 0 && static_cast<size_t>(mat.emissiveTexture.index) < model.textures.size())
    {
        const int img_index_e = model.textures[static_cast<size_t>(mat.emissiveTexture.index)].source;
        if (img_index_e >= 0 && static_cast<size_t>(img_index_e) < model.images.size())
        {
            const auto& img = model.images[static_cast<size_t>(img_index_e)];
            if (!img.uri.empty())
            {
                const fs::path rel_dir = fs::path(gltf_relpath).parent_path();
                const auto rel = (rel_dir / img.uri).generic_string();
                auto tex_name = name + ".emissive";
                m_textures.load_texture(tex_name, rel, TextureColorSpace::Srgb);
                material->set_emissive(m_textures[tex_name]);
            }
        }
    }

    auto& ref = *material;
    m_materials[name] = std::move(material);

    // Provide default textures for any missing maps
    // baseColor default: white (sRGB)
    m_textures.ensure_texture_rgba_1x1("__default.baseColor.white", 255, 255, 255, 255, TextureColorSpace::Srgb);
    if (ref.base_color() == nullptr)
    {
        ref.set_base_color(m_textures["__default.baseColor.white"]);
    }

    // normal default: (0.5, 0.5, 1.0) linear
    m_textures.ensure_texture_rgba_1x1("__default.normal.flat", 128, 128, 255, 255, TextureColorSpace::Linear);
    if (ref.normal() == nullptr)
    {
        ref.set_normal(m_textures["__default.normal.flat"]);
    }

    // metallic-roughness default: occlusion=1, roughness=1, metallic=0 (linear)
    m_textures.ensure_texture_rgba_1x1("__default.mr.neutral", 255, 255, 0, 255, TextureColorSpace::Linear);
    if (ref.metallic_roughness() == nullptr)
    {
        ref.set_metallic_roughness(m_textures["__default.mr.neutral"]);
    }

    // emissive default: black (sRGB)
    m_textures.ensure_texture_rgba_1x1("__default.emissive.black", 0, 0, 0, 255, TextureColorSpace::Srgb);
    if (ref.emissive() == nullptr)
    {
        ref.set_emissive(m_textures["__default.emissive.black"]);
    }
    return ref;
}

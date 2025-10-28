#include "vulkan/material_factory.h"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <format>
#include <tiny_gltf.h>

using namespace steeplejack;

MaterialFactory::MaterialFactory(TextureFactory& textures) : m_textures(textures), m_materials() {}

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
    m_textures.load_texture(tex_name, texture_relpath);

    auto material = std::make_unique<Material>();
    material->set_base_color(m_textures[tex_name]);

    auto& ref = *material;
    m_materials[name] = std::move(material);
    return ref;
}

Material&
MaterialFactory::load_gltf_material(const std::string& name, const std::string& gltf_relpath, int material_index)
{
    namespace fs = std::filesystem;

    const fs::path full_path = fs::path("assets/textures") / gltf_relpath;

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err, warn;

    const auto ext = full_path.extension().string();
    bool loaded = false;
    if (ext == ".glb" || ext == ".GLB")
        loaded = loader.LoadBinaryFromFile(&model, &err, &warn, full_path.string());
    else
        loaded = loader.LoadASCIIFromFile(&model, &err, &warn, full_path.string());

    if (!warn.empty())
        spdlog::warn("tinygltf warn {}: {}", full_path.string(), warn);
    if (!loaded)
        throw std::runtime_error(std::format("Failed to load glTF {}: {}", full_path.string(), err));

    if (material_index < 0 || static_cast<size_t>(material_index) >= model.materials.size())
        throw std::runtime_error(std::format("Material index {} out of range for {}", material_index, full_path.string()));

    const auto& mat = model.materials[static_cast<size_t>(material_index)];

    auto material = std::make_unique<Material>();

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
        material->set_alpha_mode(AlphaMode::Mask);
    else if (mat.alphaMode == "BLEND")
        material->set_alpha_mode(AlphaMode::Blend);
    else
        material->set_alpha_mode(AlphaMode::Opaque);
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
                m_textures.load_texture(tex_name, rel_image);
                material->set_base_color(m_textures[tex_name]);
            }
            else
            {
                throw std::runtime_error(std::format(
                    "Embedded baseColor image not supported in first pass for {}", full_path.string()));
            }
        }
    }

    auto& ref = *material;
    m_materials[name] = std::move(material);
    return ref;
}


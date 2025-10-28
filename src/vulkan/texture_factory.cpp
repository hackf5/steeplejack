#include "vulkan/texture_factory.h"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <format>
#include <tiny_gltf.h>

using namespace steeplejack;

TextureFactory::TextureFactory(const Device& device, const Sampler& sampler, const AdhocQueues& adhoc_queues) :
    m_device(device), m_sampler(sampler), m_adhoc_queues(adhoc_queues), m_textures()
{
}

void TextureFactory::load_texture(const std::string& name,
                                  const std::string& texture_name,
                                  TextureColorSpace color_space)
{
    m_textures[name] = std::make_unique<Texture>(m_device, m_sampler, m_adhoc_queues, texture_name, color_space);
}

void TextureFactory::load_texture_from_gltf(const std::string& name, const std::string& gltf_relpath)
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
        spdlog::warn("tinygltf warning while loading {}: {}", full_path.string(), warn);
    }
    if (!loaded)
    {
        throw std::runtime_error(std::format("Failed to load glTF {}: {}", full_path.string(), err));
    }

    if (model.materials.empty())
    {
        throw std::runtime_error(std::format("No materials in glTF {}", full_path.string()));
    }

    const auto tex_index = model.materials[0].pbrMetallicRoughness.baseColorTexture.index;
    if (tex_index < 0 || static_cast<size_t>(tex_index) >= model.textures.size())
    {
        throw std::runtime_error(std::format("glTF {} has no baseColor texture", full_path.string()));
    }

    const auto img_index = model.textures[static_cast<size_t>(tex_index)].source;
    if (img_index < 0 || static_cast<size_t>(img_index) >= model.images.size())
    {
        throw std::runtime_error(std::format("glTF {} baseColor texture has no valid image", full_path.string()));
    }

    const auto& image = model.images[static_cast<size_t>(img_index)];

    // Simple pass: expect an external image URI (e.g., JPG). Reuse existing loader.
    if (!image.uri.empty())
    {
        // Build a path relative to assets/textures/
        const fs::path rel_dir = fs::path(gltf_relpath).parent_path();
        const fs::path rel_image = rel_dir / image.uri;

        spdlog::info("Loading baseColor from glTF: {} -> {}", full_path.string(), rel_image.generic_string());
        load_texture(name, rel_image.generic_string(), TextureColorSpace::Srgb);
        return;
    }

    // If the image is embedded (no URI), we could upload from memory. For the first pass, fail loudly.
    throw std::runtime_error(std::format(
        "Embedded images not supported in first pass for {}. Expected external image URI.", full_path.string()));
}

void TextureFactory::remove_texture(const std::string& name)
{
    m_textures.erase(name);
}

void TextureFactory::clear()
{
    m_textures.clear();
}

Texture* TextureFactory::operator[](const std::string& name)
{
    auto it = m_textures.find(name);
    if (it == m_textures.end())
    {
        throw std::runtime_error(std::format("Texture {} not found", name));
    }

    return it->second.get();
}

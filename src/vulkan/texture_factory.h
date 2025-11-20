#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "vulkan/texture_types.h"

namespace steeplejack
{
class AdhocQueues;
class Device;
class Sampler;
class Texture;

class TextureFactory
{
  private:
    const Device& m_device;
    const Sampler& m_sampler;
    const AdhocQueues& m_adhoc_queues;

    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

  public:
    TextureFactory(const Device& device, const Sampler& sampler, const AdhocQueues& adhoc_queues);

    TextureFactory(const TextureFactory&) = delete;
    TextureFactory& operator=(const TextureFactory&) = delete;
    TextureFactory(TextureFactory&&) = delete;
    TextureFactory& operator=(TextureFactory&&) = delete;
    ~TextureFactory() = default;

    void load_texture(
        const std::string& name,
        const std::string& texture_name,
        TextureColorSpace color_space);

    // Load the baseColor (albedo) texture referenced by a glTF/GLB file.
    // gltf_relpath is relative to assets/textures/ (e.g., "rock_wall_15_1k/rock_wall_15_1k.gltf").
    void load_texture_from_gltf(const std::string& name, const std::string& gltf_relpath);

    void remove_texture(const std::string& name);

    void clear();

    Texture* operator[](const std::string& name);

    bool has(const std::string& name) const;

    // Ensure a 1x1 RGBA texture exists with given color
    void ensure_texture_rgba_1x1(
        const std::string& name, uint8_t r, uint8_t g, uint8_t b, uint8_t a, TextureColorSpace color_space);
};
} // namespace steeplejack

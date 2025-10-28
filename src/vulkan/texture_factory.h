#pragma once

#include "vulkan/adhoc_queues.h"
#include "vulkan/device.h"
#include "vulkan/sampler.h"
#include "vulkan/texture.h"

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <unordered_map>

namespace steeplejack
{
class TextureFactory
{
  private:
    const Device& m_device;
    const Sampler& m_sampler;
    const AdhocQueues& m_adhoc_queues;

    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

  public:
    TextureFactory(const Device& device, const Sampler& sampler, const AdhocQueues& adhoc_queues);

    void load_texture(const std::string& name,
                      const std::string& texture_name,
                      TextureColorSpace color_space = TextureColorSpace::Srgb);

    // Load the baseColor (albedo) texture referenced by a glTF/GLB file.
    // gltf_relpath is relative to assets/textures/ (e.g., "rock_wall_15_1k/rock_wall_15_1k.gltf").
    void load_texture_from_gltf(const std::string& name, const std::string& gltf_relpath);

    void remove_texture(const std::string& name);

    void clear();

    Texture* operator[](const std::string& name);
};
} // namespace steeplejack

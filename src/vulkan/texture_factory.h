#pragma once

#include "vulkan/adhoc_queues.h"
#include "vulkan/device.h"
#include "vulkan/sampler.h"
#include "vulkan/texture.h"

#include <cstdint>
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

    TextureFactory(const TextureFactory&) = delete;
    TextureFactory& operator=(const TextureFactory&) = delete;
    TextureFactory(TextureFactory&&) = delete;
    TextureFactory& operator=(TextureFactory&&) = delete;
    ~TextureFactory() = default;

    void load_texture(
        const std::string& name,
        const std::string& texture_name,
        TextureColorSpace color_space = TextureColorSpace::Srgb);

    // Load the baseColor (albedo) texture referenced by a glTF/GLB file.
    // gltf_relpath is relative to assets/textures/ (e.g., "rock_wall_15_1k/rock_wall_15_1k.gltf").
    void load_texture_from_gltf(const std::string& name, const std::string& gltf_relpath);

    void remove_texture(const std::string& name);

    void clear();

    Texture* operator[](const std::string& name);

    bool has(const std::string& name) const
    {
        return m_textures.contains(name);
    }

    // Ensure a 1x1 RGBA texture exists with given color
    void ensure_texture_rgba_1x1(
        const std::string& name, uint8_t r, uint8_t g, uint8_t b, uint8_t a, TextureColorSpace color_space)
    {
        if (has(name))
        {
            return;
        }

        const std::array<std::byte, 4> px{std::byte{r}, std::byte{g}, std::byte{b}, std::byte{a}};
        m_textures[name] = std::make_unique<Texture>(
            m_device,
            m_sampler,
            m_adhoc_queues,
            1,
            1,
            color_space,
            std::span<const std::byte>(px.data(), px.size()));
    }
};
} // namespace steeplejack

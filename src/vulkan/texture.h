#pragma once

#include "device.h"
#include "image.h"
#include "image_view.h"
#include "sampler.h"

#include <cstdint>
#include <span>
#include <string>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class AdhocQueues;

enum class TextureColorSpace : std::uint8_t
{
    Srgb,
    Linear
};

class Texture
{
  private:
    const Device* m_device;
    const std::string m_name;
    Image m_image;
    const ImageView m_image_view;
    VkDescriptorImageInfo m_image_descriptor_info;
    const TextureColorSpace m_color_space;

  public:
    Texture(
        const Device& device,
        const Sampler& sampler,
        const AdhocQueues& adhoc_queues,
        std::string name,
        TextureColorSpace color_space = TextureColorSpace::Srgb);

    // Construct from raw RGBA8 pixel data (width*height*4 bytes)
    Texture(
        const Device& device,
        const Sampler& sampler,
        const AdhocQueues& adhoc_queues,
        int width,
        int height,
        TextureColorSpace color_space,
        std::span<const std::byte> rgba_pixels);

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;
    ~Texture() = default;

    VkDescriptorImageInfo* descriptor();
};
} // namespace steeplejack

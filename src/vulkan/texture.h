#pragma once

#include "adhoc_queues.h"
#include "buffer/buffer.h"
#include "buffer/buffer_host.h"
#include "descriptor_set_layout.h"
#include "device.h"
#include "graphics_pipeline.h"
#include "image.h"
#include "image_view.h"
#include "sampler.h"
#include "swapchain.h"
#include "util/no_copy_or_move.h"

#include <memory>
#include <string>
#include <vector>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace steeplejack
{
enum class TextureColorSpace
{
    Srgb,
    Linear
};

class Texture : public NoCopyOrMove
{
  private:
    const Device& m_device;
    const std::string m_name;
    const std::unique_ptr<Image> m_image;
    const ImageView m_image_view;
    VkDescriptorImageInfo m_image_descriptor_info;
    const TextureColorSpace m_color_space;

    std::unique_ptr<Buffer> create_staging_buffer(const std::string& name, int& width, int& height);
    std::unique_ptr<Image> create_image(const AdhocQueues& adhoc_queues, TextureColorSpace color_space);
    VkDescriptorImageInfo create_image_descriptor_info(const Sampler& sampler);

    void transition_image_layout(const AdhocQueues& adhoc_queues, VkImageLayout old_layout, VkImageLayout new_layout);

    void copy_staging_buffer_to_image(const Buffer& staging_buffer, const AdhocQueues& adhoc_queues);

  public:
    Texture(const Device& device,
            const Sampler& sampler,
            const AdhocQueues& adhoc_queues,
            std::string name,
            TextureColorSpace color_space = TextureColorSpace::Srgb);

    // Construct from raw RGBA8 pixel data (width*height*4 bytes)
    Texture(const Device& device,
            const Sampler& sampler,
            const AdhocQueues& adhoc_queues,
            int width,
            int height,
            TextureColorSpace color_space,
            std::span<const std::byte> rgba_pixels);

    VkDescriptorImageInfo* descriptor()
    {
        return &m_image_descriptor_info;
    }
};
} // namespace steeplejack

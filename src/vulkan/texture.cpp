#include "texture.h"

#include "adhoc_queues.h"
#include "buffer/staging_buffer.h"
#include "spdlog/spdlog.h"
#include "stb_image.h"

#include <cstddef>
#include <span>
#include <utility>

using namespace steeplejack;

namespace
{
Buffer create_staging_buffer(const Device& device, const std::string& name, int& width, int& height)
{
    auto file_name = "assets/textures/" + name;

    spdlog::info("Loading image: {}", file_name);
    int channels = 0;
    auto* pixels = stbi_load(file_name.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        throw std::runtime_error("Failed to load image " + file_name + ": " + stbi_failure_reason());
    }

    const size_t bytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4U;
    const auto bytes_view = std::span<const std::byte>(reinterpret_cast<const std::byte*>(pixels), bytes);
    StagingBuffer staging_buffer(device, bytes_view);

    stbi_image_free(pixels);

    return staging_buffer;
}

void transition_image_layout(
    Image& image, const AdhocQueues& adhoc_queues, VkImageLayout old_layout, VkImageLayout new_layout)
{
    VkCommandBuffer command_buffer = adhoc_queues.graphics().begin();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.vk();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage = 0;
    VkPipelineStageFlags destination_stage = 0;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (
        old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    adhoc_queues.graphics().submit_and_wait();
}

void copy_staging_buffer_to_image(const Buffer& staging_buffer, const AdhocQueues& adhoc_queues, Image& image)
{
    VkCommandBuffer command_buffer = adhoc_queues.transfer().begin();

    VkImageSubresourceLayers subresource = {};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = 0;
    subresource.baseArrayLayer = 0;
    subresource.layerCount = 1;

    VkOffset3D offset = {};
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;

    VkExtent3D extent = {};
    extent.width = image.image_info().width;
    extent.height = image.image_info().height;
    extent.depth = 1;

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource = subresource;
    region.imageOffset = offset;
    region.imageExtent = extent;

    vkCmdCopyBufferToImage(
        command_buffer,
        staging_buffer.vk(),
        image.vk(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    adhoc_queues.transfer().submit_and_wait();
}

Image create_image(
    const Device& device, const std::string& name, const AdhocQueues& adhoc_queues, TextureColorSpace color_space)
{
    int width = 0;
    int height = 0;
    auto staging_buffer = create_staging_buffer(device, name, width, height);
    const VkFormat format =
        (color_space == TextureColorSpace::Srgb) ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
    Image image(
        device,
        width,
        height,
        format,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_TILING_OPTIMAL);

    transition_image_layout(image, adhoc_queues, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    copy_staging_buffer_to_image(staging_buffer, adhoc_queues, image);

    transition_image_layout(
        image,
        adhoc_queues,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return image;
}

VkDescriptorImageInfo create_image_descriptor_info(const Sampler& sampler, const ImageView& image_view)
{
    VkDescriptorImageInfo image_info = {};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = image_view.vk();
    image_info.sampler = sampler.vk();

    return image_info;
}
} // namespace

Texture::Texture(
    const Device& device,
    const Sampler& sampler,
    const AdhocQueues& adhoc_queues,
    std::string name,
    TextureColorSpace color_space) :
    m_device(&device),
    m_name(std::move(name)),
    m_image(create_image(*m_device, m_name, adhoc_queues, color_space)),
    m_image_view(*m_device, m_image, VK_IMAGE_ASPECT_COLOR_BIT),
    m_image_descriptor_info(create_image_descriptor_info(sampler, m_image_view)),
    m_color_space(color_space)
{
}
Texture::Texture(
    const Device& device,
    const Sampler& sampler,
    const AdhocQueues& adhoc_queues,
    int width,
    int height,
    TextureColorSpace color_space,
    std::span<const std::byte> rgba_pixels) :
    m_device(&device),
    m_name("<memory>"),
    m_image(
        *m_device,
        width,
        height,
        color_space == TextureColorSpace::Srgb ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_TILING_OPTIMAL),
    m_image_view(*m_device, m_image, VK_IMAGE_ASPECT_COLOR_BIT),
    m_image_descriptor_info(create_image_descriptor_info(sampler, m_image_view)),
    m_color_space(color_space)
{
    // Transition, upload, transition
    transition_image_layout(m_image, adhoc_queues, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    StagingBuffer const staging_buffer(*m_device, rgba_pixels);
    copy_staging_buffer_to_image(staging_buffer, adhoc_queues, m_image);

    transition_image_layout(
        m_image,
        adhoc_queues,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkDescriptorImageInfo* Texture::descriptor()
{
    return &m_image_descriptor_info;
}

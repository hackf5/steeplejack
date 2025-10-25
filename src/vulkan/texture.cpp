#include "texture.h"

#include "stb_image.h"
#include "spdlog/spdlog.h"

#include "buffer/staging_buffer.h"

using namespace levin;

Texture::Texture(
    const Device &device,
    const Sampler &sampler,
    const AdhocQueues &adhoc_queues,
    const std::string &name):
    m_device(device),
    m_name(name),
    m_image(create_image(adhoc_queues)),
    m_image_view(m_device, *m_image, VK_IMAGE_ASPECT_COLOR_BIT),
    m_image_descriptor_info(create_image_descriptor_info(sampler))
{
}

std::unique_ptr<Buffer> Texture::create_staging_buffer(
    const std::string &name,
    int &width,
    int &height)
{
    auto file_name = "assets/textures/" + name;

    spdlog::info("Loading image: {}", file_name);
    int channels;
    auto pixels = stbi_load(file_name.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!pixels)
    {
        throw std::runtime_error("Failed to load image " + file_name + ": " + stbi_failure_reason());
    }

    auto staging_buffer = std::make_unique<StagingBuffer>(
        m_device,
        pixels,
        pixels + (width * height * channels));

    stbi_image_free(pixels);

    return staging_buffer;
}

std::unique_ptr<Image> Texture::create_image(const AdhocQueues &adhoc_queues)
{
    int width, height;
    auto staging_buffer = create_staging_buffer(m_name, width, height);
    auto image = std::make_unique<Image>(
        m_device,
        width,
        height,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_TILING_OPTIMAL);

    transition_image_layout(
        adhoc_queues,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    copy_staging_buffer_to_image(*staging_buffer, adhoc_queues);

    transition_image_layout(
        adhoc_queues,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return image;
}

void Texture::transition_image_layout(
    const AdhocQueues &adhoc_queues,
    VkImageLayout old_layout,
    VkImageLayout new_layout)
{
    VkCommandBuffer command_buffer = adhoc_queues.graphics().begin();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = *m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED
        && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
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

    vkCmdPipelineBarrier(
        command_buffer,
        source_stage,
        destination_stage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    adhoc_queues.graphics().submit_and_wait();
}

void Texture::copy_staging_buffer_to_image(
    const Buffer &staging_buffer,
    const AdhocQueues &adhoc_queues)
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
    extent.width = m_image->image_info().width;
    extent.height = m_image->image_info().height;
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
        staging_buffer,
        *m_image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    adhoc_queues.transfer().submit_and_wait();
}

VkDescriptorImageInfo Texture::create_image_descriptor_info(const Sampler &sampler)
{
    VkDescriptorImageInfo image_info = {};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = m_image_view;
    image_info.sampler = sampler;

    return image_info;
}
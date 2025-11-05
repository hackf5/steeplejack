#include "shadow_map_array.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

ShadowMapArray::ShadowMapArray(
    const Device& device, const AdhocQueues& adhoc_queues, uint32_t layers, uint32_t resolution, VkFormat format) :
    m_device(device),
    m_adhoc_queues(adhoc_queues),
    m_image(
        device,
        resolution,
        resolution,
        format,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_SAMPLE_COUNT_1_BIT,
        layers),
    m_array_image_view(device, m_image, VK_IMAGE_ASPECT_DEPTH_BIT),
    m_layer_image_views(
        [&]()
        {
            std::vector<std::unique_ptr<ImageView>> views;
            views.reserve(layers);
            for (uint32_t layer = 0; layer < layers; ++layer)
            {
                views.emplace_back(std::make_unique<ImageView>(device, m_image, VK_IMAGE_ASPECT_DEPTH_BIT, layer));
            }
            return views;
        }()),
    m_memory_barrier(create_memory_barrier())
{
    spdlog::info("Creating Shadow Map Array");

    VkCommandBuffer command_buffer = m_adhoc_queues.graphics().begin();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = m_image.image_info().array_layers;
    barrier.image = m_image;

    vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    m_adhoc_queues.graphics().submit_and_wait();
}

VkImageMemoryBarrier ShadowMapArray::create_memory_barrier()
{
    VkImageMemoryBarrier barrier{};

    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image; // from ShadowMapArray
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = m_image.image_info().array_layers;

    return barrier;
}
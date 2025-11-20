#pragma once

#include "vulkan/adhoc_queues.h"
#include "vulkan/device.h"
#include "vulkan/image.h"
#include "vulkan/image_view.h"

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowMapArray
{
  private:
    const Device& m_device;
    const AdhocQueues& m_adhoc_queues;
    const Image m_image;
    const ImageView m_array_image_view;
    const std::vector<std::unique_ptr<ImageView>> m_layer_image_views;
    const VkImageMemoryBarrier m_memory_barrier;

  public:
    ShadowMapArray(
        const Device& device,
        const AdhocQueues& adhoc_queues,
        uint32_t layers,
        uint32_t resolution = 1024,
        VkFormat format = VK_FORMAT_D32_SFLOAT);
    ~ShadowMapArray() = default;

    ShadowMapArray(const ShadowMapArray&) = delete;
    ShadowMapArray& operator=(const ShadowMapArray&) = delete;
    ShadowMapArray(ShadowMapArray&&) = delete;
    ShadowMapArray& operator=(ShadowMapArray&&) = delete;

    [[nodiscard]] uint32_t layers() const;
    [[nodiscard]] uint32_t resolution() const;
    [[nodiscard]] VkImageView array_view() const;
    [[nodiscard]] VkImageView layer_view(uint32_t layer) const;

    void pipeline_barrier(VkCommandBuffer command_buffer) const;
};
} // namespace steeplejack

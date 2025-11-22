#pragma once

#include "vulkan/image.h"
#include "vulkan/image_view.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class Device;
class Swapchain;

class DepthBuffer
{
  private:
    const Image m_image;
    const ImageView m_image_view;

  public:
    DepthBuffer(const Device& device, const Swapchain& swapchain);

    DepthBuffer(const DepthBuffer&) = delete;
    DepthBuffer& operator=(const DepthBuffer&) = delete;
    DepthBuffer(DepthBuffer&&) = delete;
    DepthBuffer& operator=(DepthBuffer&&) = delete;
    ~DepthBuffer() = default;

    [[nodiscard]] VkImageView image_view() const;
    [[nodiscard]] VkFormat format() const;
};
} // namespace steeplejack

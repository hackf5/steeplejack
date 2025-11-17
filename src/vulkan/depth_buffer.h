#pragma once

#include "device.h"
#include "image.h"
#include "image_view.h"
#include "swapchain.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
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

    [[nodiscard]] VkImageView image_view() const
    {
        return m_image_view;
    }

    [[nodiscard]] VkFormat format() const
    {
        return m_image.image_info().format;
    }
};
} // namespace steeplejack

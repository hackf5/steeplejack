#pragma once

#include "device.h"
#include "image.h"
#include "image_view.h"
#include "swapchain.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class Multisampler
{
  private:
    const Device& m_device;
    const Image m_image;
    const ImageView m_image_view;

  public:
    Multisampler(const Device& device, const Swapchain& swapchain);

    Multisampler(const Multisampler&) = delete;
    Multisampler& operator=(const Multisampler&) = delete;
    Multisampler(Multisampler&&) = delete;
    Multisampler& operator=(Multisampler&&) = delete;

    VkImageView image_view() const
    {
        return m_image_view;
    }
};
} // namespace steeplejack

#pragma once

#include "device.h"
#include "image.h"
#include "image_view.h"
#include "swapchain.h"
#include "util/no_copy_or_move.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class Multisampler : public NoCopyOrMove
{
  private:
    const Device& m_device;
    const Image m_image;
    const ImageView m_image_view;

  public:
    Multisampler(const Device& device, const Swapchain& swapchain);

    VkImageView image_view() const
    {
        return m_image_view;
    }
};
} // namespace steeplejack
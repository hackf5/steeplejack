#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "util/no_copy_or_move.h"
#include "device.h"
#include "image.h"
#include "image_view.h"
#include "swapchain.h"

namespace steeplejack
{
class DepthBuffer: public NoCopyOrMove
{
private:
    const Image m_image;
    const ImageView m_image_view;

public:
    DepthBuffer(const Device &device, const Swapchain &swapchain);

    VkImageView image_view() const { return m_image_view; }

    VkFormat format() const { return m_image.image_info().format; }
};
}
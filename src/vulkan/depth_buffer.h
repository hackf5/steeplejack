#pragma once

#include <vulkan/vulkan.h>

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"
#include "vulkan/image.h"
#include "vulkan/image_view.h"
#include "vulkan/swapchain.h"

namespace steeplejack {

class DepthBuffer : public NoCopyOrMove {
private:
    const Image image_;
    const ImageView image_view_;

public:
    DepthBuffer(const Device& device, const Swapchain& swapchain);
    VkImageView image_view() const { return image_view_; }
    VkFormat format() const { return image_.image_info().format; }
};

} // namespace steeplejack


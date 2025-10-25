#pragma once

#include <vulkan/vulkan.h>

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"
#include "vulkan/image.h"
#include "vulkan/image_view.h"
#include "vulkan/swapchain.h"

namespace steeplejack {

class Multisampler : public NoCopyOrMove {
private:
    const Device& device_;
    const Image image_;
    const ImageView image_view_;

public:
    Multisampler(const Device& device, const Swapchain& swapchain);
    VkImageView image_view() const { return image_view_; }
};

} // namespace steeplejack


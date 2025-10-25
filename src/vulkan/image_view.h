#pragma once

#include <vulkan/vulkan.h>

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"
#include "vulkan/image.h"

namespace steeplejack {

class ImageView : public NoCopyOrMove {
private:
    const Device& device_;
    const Image& image_;
    const VkImageAspectFlags aspect_;
    const VkImageView image_view_;

    VkImageView create_image_view();

public:
    ImageView(const Device& device, const Image& image, VkImageAspectFlags aspect_mask);
    ~ImageView();

    operator VkImageView() const { return image_view_; }
};

} // namespace steeplejack


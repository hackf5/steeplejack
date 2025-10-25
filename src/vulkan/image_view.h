#pragma once

#include <vulkan/vulkan.h>

#include "util/no_copy_or_move.h"
#include "device.h"
#include "image.h"

namespace levin
{
class ImageView: public NoCopyOrMove
{
private:
    const Device &m_device;
    const Image &m_image;
    const VkImageAspectFlags m_aspect_mask;
    const VkImageView m_image_view;

    VkImageView create_image_view();

public:
    ImageView(
        const Device &device,
        const Image &image,
        VkImageAspectFlags aspect_mask);
    ~ImageView();

    operator VkImageView() const { return m_image_view; }
};
}
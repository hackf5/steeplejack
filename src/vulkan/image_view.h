#pragma once

#include "device.h"
#include "image.h"
#include "util/no_copy_or_move.h"

#include <optional>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ImageView : public NoCopyOrMove
{
  private:
    const Device& m_device;
    const Image& m_image;
    const VkImageView m_image_view;

    VkImageView create_image_view(VkImageAspectFlags aspect_mask, std::optional<uint32_t> base_layer);

  public:
    // base_layer: if specified, creates a view for a single layer in a 2D array starting at base_layer;
    ImageView(
        const Device& device,
        const Image& image,
        VkImageAspectFlags aspect_mask,
        std::optional<uint32_t> base_layer = std::nullopt);
    ~ImageView();

    operator VkImageView() const
    {
        return m_image_view;
    }
};
} // namespace steeplejack
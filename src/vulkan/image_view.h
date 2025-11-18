#pragma once

#include "device.h"
#include "image.h"

#include <optional>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ImageView
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

    ImageView(const ImageView&) = delete;
    ImageView& operator=(const ImageView&) = delete;
    ImageView(ImageView&&) = delete;
    ImageView& operator=(ImageView&&) = delete;

    [[nodiscard]] VkImageView vk() const
    {
        return m_image_view;
    }
};
} // namespace steeplejack

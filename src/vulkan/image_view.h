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
    const VkImageView m_image_view;

  public:
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

    [[nodiscard]] VkImageView vk() const;
};
} // namespace steeplejack

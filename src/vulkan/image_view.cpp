#include "image_view.h"

#include "spdlog/spdlog.h"

#include <stdexcept>

using namespace steeplejack;

namespace
{
[[nodiscard]] VkImageView create_image_view(
    const Device& device, const Image& image, VkImageAspectFlags aspect_mask, std::optional<uint32_t> base_layer)
{
    spdlog::info("Creating Image View");

    auto array_layers = image.image_info().array_layers;

    // base_layer can only be specified for array images
    assert(!base_layer.has_value() || (array_layers > 1 && base_layer.value() < array_layers));

    VkImageViewCreateInfo image_view_info = {};
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_info.image = image.vk();

    image_view_info.viewType =
        !base_layer.has_value() && array_layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;

    image_view_info.format = image.image_info().format;
    image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.subresourceRange.aspectMask = aspect_mask;
    image_view_info.subresourceRange.baseMipLevel = 0;
    image_view_info.subresourceRange.levelCount = 1;
    image_view_info.subresourceRange.baseArrayLayer = base_layer.value_or(0);
    image_view_info.subresourceRange.layerCount = base_layer.has_value() ? 1 : array_layers;

    VkImageView image_view = nullptr;
    if (vkCreateImageView(device.vk(), &image_view_info, nullptr, &image_view) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image view");
    }

    return image_view;
}
} // namespace

ImageView::ImageView(
    const Device& device, const Image& image, VkImageAspectFlags aspect_mask, std::optional<uint32_t> base_layer) :
    m_device(device), m_image(image), m_image_view(create_image_view(device, image, aspect_mask, base_layer))
{
}

ImageView::~ImageView()
{
    spdlog::info("Destroying Image View");

    vkDestroyImageView(m_device.vk(), m_image_view, nullptr);
}

VkImageView ImageView::vk() const
{
    return m_image_view;
}

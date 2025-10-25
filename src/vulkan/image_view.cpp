#include "image_view.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

ImageView::ImageView(
    const Device &device,
    const Image &image,
    VkImageAspectFlags aspect_mask):
    m_device(device),
    m_image(image),
    m_aspect_mask(aspect_mask),
    m_image_view(create_image_view())
{
}

ImageView::~ImageView()
{
    spdlog::info("Destroying image view");

    vkDestroyImageView(m_device, m_image_view, nullptr);
}

VkImageView ImageView::create_image_view()
{
    VkImageViewCreateInfo image_view_info = {};
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_info.image = m_image;
    image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format = m_image.image_info().format;
    image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.subresourceRange.aspectMask = m_aspect_mask;
    image_view_info.subresourceRange.baseMipLevel = 0;
    image_view_info.subresourceRange.levelCount = 1;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    if (vkCreateImageView(m_device, &image_view_info, nullptr, &image_view) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image view");
    }

    return image_view;
}
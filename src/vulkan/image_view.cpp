#include "vulkan/image_view.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

ImageView::ImageView(const Device& device, const Image& image, VkImageAspectFlags aspect_mask)
    : device_(device), image_(image), aspect_(aspect_mask), image_view_(create_image_view()) {}

ImageView::~ImageView() {
    spdlog::info("Destroying image view");
    vkDestroyImageView(device_, image_view_, nullptr);
}

VkImageView ImageView::create_image_view() {
    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = image_;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = image_.image_info().format;
    info.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                       VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    info.subresourceRange.aspectMask = aspect_;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;

    VkImageView view{};
    if (vkCreateImageView(device_, &info, nullptr, &view) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view");
    }
    return view;
}


#include "vulkan/swapchain.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

Swapchain::Swapchain(const Device& device)
    : device_(device),
      swapchain_(create_swapchain()),
      images_(swapchain_.get_images().value()),
      image_views_(swapchain_.get_image_views().value()),
      viewport_(create_viewport()),
      scissor_(create_scissor()) {}

Swapchain::~Swapchain() {
    spdlog::info("Destroying Swapchain");
    for (auto iv : image_views_) {
        vkDestroyImageView(device_, iv, nullptr);
    }
    vkb::destroy_swapchain(swapchain_);
}

vkb::Swapchain Swapchain::create_swapchain() {
    spdlog::info("Creating Swapchain");
    vkb::SwapchainBuilder builder{device_};
    auto ret = builder.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR).build();
    if (!ret) {
        throw std::runtime_error("Failed to create swapchain: " + ret.error().message());
    }
    auto sc = ret.value();
    if (sc.image_count < 3) {
        throw std::runtime_error("Swapchain image count must be at least 3");
    }
    return sc;
}

VkViewport Swapchain::create_viewport() {
    VkViewport vp{};
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.width = static_cast<float>(swapchain_.extent.width);
    vp.height = static_cast<float>(swapchain_.extent.height);
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
    return vp;
}

VkRect2D Swapchain::create_scissor() {
    VkRect2D sc{};
    sc.offset = {0, 0};
    sc.extent = swapchain_.extent;
    return sc;
}


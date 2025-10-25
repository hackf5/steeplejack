#pragma once

#include <vector>

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"

namespace steeplejack {

class Swapchain : NoCopyOrMove {
private:
    const Device& device_;
    vkb::Swapchain swapchain_{};
    const std::vector<VkImage> images_;
    const std::vector<VkImageView> image_views_;
    const VkViewport viewport_;
    const VkRect2D scissor_;

    vkb::Swapchain create_swapchain();
    VkViewport create_viewport();
    VkRect2D create_scissor();

public:
    explicit Swapchain(const Device& device);
    ~Swapchain();

    operator VkSwapchainKHR() const { return swapchain_.swapchain; }
    VkExtent2D extent() const { return swapchain_.extent; }
    float aspect_ratio() const {
        return static_cast<float>(swapchain_.extent.width) / static_cast<float>(swapchain_.extent.height);
    }
    uint32_t image_count() const { return swapchain_.image_count; }
    VkFormat image_format() const { return swapchain_.image_format; }
    const VkViewport& viewport() const { return viewport_; }
    const VkRect2D& scissor() const { return scissor_; }
    const VkImage& image(size_t idx) const { return images_[idx]; }
    const VkImageView& image_view(size_t idx) const { return image_views_[idx]; }
    void clip(VkCommandBuffer cmd) const {
        vkCmdSetViewport(cmd, 0, 1, &viewport_);
        vkCmdSetScissor(cmd, 0, 1, &scissor_);
    }
};

} // namespace steeplejack


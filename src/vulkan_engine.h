#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "vulkan_context.h"

namespace steeplejack {

class VulkanEngine {
private:
    std::unique_ptr<VulkanContext> context_;
    uint32_t current_frame_ = 0;

    void draw_frame();
    void recreate_swapchain();
    void render(VkFramebuffer framebuffer);
    void next_frame() { current_frame_ = (current_frame_ + 1) % Device::max_frames_in_flight; }

public:
    explicit VulkanEngine(std::unique_ptr<VulkanContext> context);
    void run();
};

} // namespace steeplejack


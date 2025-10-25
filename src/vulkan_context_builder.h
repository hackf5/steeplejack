#pragma once

#include <functional>
#include <memory>

#include "util/no_copy_or_move.h"
#include "vulkan_context.h"

namespace steeplejack {

class VulkanContextBuilder : NoCopyOrMove {
private:
    std::unique_ptr<VulkanContext> ctx_;

public:
    VulkanContextBuilder() : ctx_(std::make_unique<VulkanContext>()) {}
    explicit VulkanContextBuilder(std::unique_ptr<VulkanContext> context) : ctx_(std::move(context)) {}

    VulkanContextBuilder& add_window(int width, int height, const std::string& title);
    VulkanContextBuilder& add_device(bool enable_validation_layers = true);
    VulkanContextBuilder& add_adhoc_queues();
    VulkanContextBuilder& add_graphics_queue();
    VulkanContextBuilder& add_swapchain();
    VulkanContextBuilder& add_depth_buffer();
    VulkanContextBuilder& add_render_pass();
    VulkanContextBuilder& add_framebuffers();

    std::unique_ptr<VulkanContext> build();
};

} // namespace steeplejack


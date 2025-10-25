#include "vulkan_context_builder.h"

using namespace steeplejack;

VulkanContextBuilder& VulkanContextBuilder::add_window(int width, int height, const std::string& title) {
    ctx_->window_ = std::make_unique<Window>(width, height, title);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_device(bool enable_validation_layers) {
    ctx_->device_ = std::make_unique<Device>(*ctx_->window_, enable_validation_layers);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_adhoc_queues() {
    ctx_->adhoc_queues_ = std::make_unique<AdhocQueues>(*ctx_->device_);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_graphics_queue() {
    ctx_->graphics_queue_ = std::make_unique<GraphicsQueue>(*ctx_->device_);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_swapchain() {
    if (ctx_->swapchain_.get() != nullptr) {
        ctx_->framebuffers_.reset();
        ctx_->render_pass_.reset();
        ctx_->depth_buffer_.reset();
        ctx_->swapchain_.reset();
    }
    ctx_->swapchain_ = std::make_unique<Swapchain>(*ctx_->device_);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_depth_buffer() {
    ctx_->depth_buffer_ = std::make_unique<DepthBuffer>(*ctx_->device_, *ctx_->swapchain_);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_render_pass() {
    ctx_->render_pass_ =
        std::make_unique<RenderPass>(*ctx_->device_, *ctx_->swapchain_, *ctx_->depth_buffer_);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_framebuffers() {
    ctx_->framebuffers_ = std::make_unique<Framebuffers>(
        *ctx_->device_, *ctx_->swapchain_, *ctx_->render_pass_, *ctx_->depth_buffer_);
    return *this;
}

std::unique_ptr<VulkanContext> VulkanContextBuilder::build() { return std::move(ctx_); }


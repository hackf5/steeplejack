#include "vulkan_engine.h"
#include "vulkan_context_builder.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

VulkanEngine::VulkanEngine(std::unique_ptr<VulkanContext> context) : context_(std::move(context)) {}

void VulkanEngine::run() {
    spdlog::info("Vulkan Engine is running");
    while (!context_->window().should_close()) {
        context_->window().poll_events();
        draw_frame();
    }
    context_->device().wait_idle();
}

void VulkanEngine::recreate_swapchain() {
    context_->window().wait_resize();
    context_->device().wait_idle();
    auto c = VulkanContextBuilder(std::move(context_))
                 .add_swapchain()
                 .add_depth_buffer()
                 .add_render_pass()
                 .add_framebuffers()
                 .build();
    context_ = std::move(c);
}

void VulkanEngine::draw_frame() {
    auto framebuffer = context_->graphics_queue().prepare_framebuffer(current_frame_, context_->swapchain(),
                                                                     context_->framebuffers());
    if (!framebuffer) {
        recreate_swapchain();
        return;
    }
    render(framebuffer);
    if (!context_->graphics_queue().present_framebuffer()) {
        recreate_swapchain();
    }
    next_frame();
}

void VulkanEngine::render(VkFramebuffer framebuffer) {
    auto cmd = context_->graphics_queue().begin_command();
    context_->render_pass().begin(cmd, framebuffer);
    // No pipeline/geometry yet; just clear and present
    context_->render_pass().end(cmd);
    context_->graphics_queue().submit_command();
}

#pragma once

#include <functional>

#include "util/no_copy_or_move.h"
#include "vulkan/descriptor_set_layout_builder.h"
#include "vulkan_context.h"

namespace steeplejack
{
class VulkanContextBuilder: NoCopyOrMove
{
private:
    std::unique_ptr<VulkanContext> m_context;

public:
    VulkanContextBuilder(): m_context(std::make_unique<VulkanContext>()) {}

    VulkanContextBuilder(std::unique_ptr<VulkanContext> context): m_context(std::move(context)) {}

    VulkanContextBuilder &add_window(int width, int height, const std::string &title);

    VulkanContextBuilder &add_device(bool enable_validation_layers = true);

    VulkanContextBuilder &add_adhoc_queues();

    VulkanContextBuilder &add_graphics_queue();

    VulkanContextBuilder &add_descriptor_set_layout(
        std::function<void(DescriptorSetLayoutBuilder &)> configure);

    VulkanContextBuilder &add_graphics_buffers();

    VulkanContextBuilder &add_sampler();

    VulkanContextBuilder &add_texture_factory();

    VulkanContextBuilder &add_scene(
        std::function<std::unique_ptr<RenderScene>(const Device &)> scene_factory);

    VulkanContextBuilder &add_swapchain();

    VulkanContextBuilder &add_depth_buffer();

    VulkanContextBuilder &add_render_pass();

    VulkanContextBuilder &add_framebuffers();

    VulkanContextBuilder &add_graphics_pipeline();

    VulkanContextBuilder &add_gui();

    std::unique_ptr<VulkanContext> build();
};
}
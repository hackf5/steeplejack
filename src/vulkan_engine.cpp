#include "vulkan_engine.h"

#include "glm_config.hpp"
#include "spdlog/spdlog.h"
#include "vulkan_context_builder.h"

#include <chrono>

using namespace steeplejack;

VulkanEngine::VulkanEngine(std::unique_ptr<VulkanContext> context) : m_context(std::move(context)) {}

void VulkanEngine::run()
{
    spdlog::info("Vulkan Engine is running");

    m_context->render_scene().load(m_context->device(), m_context->material_factory(), m_context->graphics_buffers());

    while (!m_context->window().should_close())
    {
        m_context->window().poll_events();
        draw_frame();
    }

    m_context->device().wait_idle();
}

void VulkanEngine::recreate_swapchain()
{
    m_context->window().wait_resize();
    m_context->device().wait_idle();

    auto context = VulkanContextBuilder(std::move(m_context))
                       .add_swapchain()
                       .add_depth_buffer()
                       .add_render_pass()
                       .add_framebuffers()
                       .add_graphics_pipeline()
                       .add_gui()
                       .build();
    m_context = std::move(context);
}

void VulkanEngine::draw_frame()
{
    m_context->gui().begin_frame();

    auto* framebuffer = m_context->graphics_queue().prepare_framebuffer(
        m_current_frame,
        m_context->swapchain(),
        m_context->framebuffers());

    if (framebuffer == nullptr)
    {
        recreate_swapchain();
        return;
    }

    m_context->render_scene().update(m_current_frame, m_context->swapchain().aspect_ratio());

    render(framebuffer);

    if (!m_context->graphics_queue().present_framebuffer())
    {
        recreate_swapchain();
    }

    next_frame();
}

void VulkanEngine::render(VkFramebuffer framebuffer)
{
    auto* command_buffer = m_context->graphics_queue().begin_command();

    m_context->render_pass().begin(command_buffer, framebuffer);

    m_context->graphics_pipeline().bind(command_buffer);
    m_context->swapchain().clip(command_buffer);
    m_context->graphics_buffers().bind(command_buffer);

    m_context->render_scene().render(command_buffer, m_current_frame, m_context->graphics_pipeline());
    steeplejack::Gui::render(command_buffer);

    m_context->render_pass().end(command_buffer);

    m_context->graphics_queue().submit_command();
}

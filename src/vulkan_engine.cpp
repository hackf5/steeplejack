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

    auto resolution = m_context->shadow_mapping().resolution();
    auto const rf = static_cast<float>(resolution);
    VkViewport const viewport{0.0F, 0.0F, rf, rf, 0.0F, 1.0F};
    VkRect2D const scissor{{0, 0}, {resolution, resolution}};
    const auto& scene_lights = m_context->render_scene().scene().lights();
    for (size_t spot_index = 0; spot_index < steeplejack::Lights::spots_size(); ++spot_index)
    {
        const auto& spot = scene_lights.spot_at(spot_index);
        if (!spot.enable)
        {
            continue;
        }

        auto* shadow_framebuffer = m_context->shadow_framebuffers().at(spot_index);
        m_context->shadow_render_pass().begin(command_buffer, shadow_framebuffer, resolution);
        m_context->shadow_pipeline().bind(command_buffer);
        m_context->graphics_buffers().bind(command_buffer);

        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);
        vkCmdSetDepthBias(command_buffer, 1.25F, 0.0F, 1.75F);

        m_context->render_scene()
            .render_shadow(command_buffer, m_current_frame, m_context->shadow_pipeline(), spot_index);

        m_context->shadow_render_pass().end(command_buffer);
    }

    m_context->shadow_mapping().write_memory_barrier(command_buffer);

    m_context->render_pass().begin(command_buffer, framebuffer);

    m_context->graphics_pipeline().bind(command_buffer);
    m_context->swapchain().clip(command_buffer);
    m_context->graphics_buffers().bind(command_buffer);

    VkDescriptorImageInfo shadow_desc{};
    shadow_desc.sampler = m_context->shadow_sampler();
    shadow_desc.imageView = m_context->shadow_mapping().array_view();
    shadow_desc.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    m_context->render_scene().render(command_buffer, m_current_frame, m_context->graphics_pipeline(), &shadow_desc);
    steeplejack::Gui::render(command_buffer);

    m_context->render_pass().end(command_buffer);

    m_context->graphics_queue().submit_command();
}

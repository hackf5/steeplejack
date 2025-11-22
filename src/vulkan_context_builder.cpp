#include "vulkan_context_builder.h"

#include "gui/gui.h"
#include "scenes/render_scene.h"
#include "vulkan/depth_buffer.h"
#include "vulkan/descriptor_set_layout.h"
#include "vulkan/device.h"
#include "vulkan/framebuffers.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_queue.h"
#include "vulkan/material_factory.h"
#include "vulkan/sampler.h"
#include "vulkan/shadow_framebuffers.h"
#include "vulkan/shadow_map_array.h"
#include "vulkan/shadow_pipeline.h"
#include "vulkan/shadow_render_pass.h"
#include "vulkan/shadow_sampler.h"
#include "vulkan/swapchain.h"
#include "vulkan/texture_factory.h"
#include "vulkan/window.h"

#include <stdexcept>
#include <string>

using namespace steeplejack;

VulkanContextBuilder& VulkanContextBuilder::add_window(int width, int height, const std::string& title)
{
    m_context->m_window = std::make_unique<Window>(width, height, title);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_device(bool enableValidationLayers)
{
    m_context->m_device = std::make_unique<Device>(*m_context->m_window, enableValidationLayers);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_adhoc_queues()
{
    m_context->m_adhoc_queues = std::make_unique<AdhocQueues>(*m_context->m_device);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_graphics_queue()
{
    m_context->m_graphics_queue = std::make_unique<GraphicsQueue>(*m_context->m_device);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_descriptor_set_layout(std::string_view layout_name)
{
    auto name = std::string(layout_name);
    if (m_context->m_descriptor_set_layouts.contains(name))
    {
        throw std::runtime_error("Descriptor set layout already registered: " + name);
    }
    m_context->m_descriptor_set_layouts.emplace(
        name,
        std::make_unique<DescriptorSetLayout>(*m_context->m_device, layout_name));
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_graphics_buffers()
{
    m_context->m_graphics_buffers = std::make_unique<GraphicsBuffers>(*m_context->m_device, *m_context->m_adhoc_queues);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_sampler()
{
    m_context->m_sampler = std::make_unique<Sampler>(*m_context->m_device);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_shadow_sampler()
{
    m_context->m_shadow_sampler = std::make_unique<ShadowSampler>(*m_context->m_device);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_texture_factory()
{
    m_context->m_texture_factory =
        std::make_unique<TextureFactory>(*m_context->m_device, *m_context->m_sampler, *m_context->m_adhoc_queues);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_material_factory()
{
    m_context->m_material_factory =
        std::make_unique<MaterialFactory>(*m_context->m_device, *m_context->m_texture_factory);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_shadow_mapping(uint32_t shadow_map_count)
{
    m_context->m_shadow_mapping =
        std::make_unique<ShadowMapArray>(*m_context->m_device, *m_context->m_adhoc_queues, shadow_map_count);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_shadow_render_pass()
{
    m_context->m_shadow_render_pass = std::make_unique<ShadowRenderPass>(*m_context->m_device);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_shadow_framebuffers()
{
    m_context->m_shadow_framebuffers = std::make_unique<ShadowFramebuffers>(
        *m_context->m_device,
        *m_context->m_shadow_mapping,
        *m_context->m_shadow_render_pass);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_shadow_pipeline(
    std::string_view layout_name, const std::string& vertex_shader, const std::string& fragment_shader)
{
    m_context->m_shadow_pipeline = std::make_unique<ShadowPipeline>(
        *m_context->m_device,
        m_context->descriptor_set_layout(layout_name),
        *m_context->m_shadow_render_pass,
        vertex_shader,
        fragment_shader);
    return *this;
}

VulkanContextBuilder&
VulkanContextBuilder::add_scene(const std::function<std::unique_ptr<RenderScene>(const Device&)>& scene_factory)
{
    m_context->m_render_scene.reset();
    m_context->m_render_scene = scene_factory(*m_context->m_device);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_swapchain()
{
    if (m_context->m_swapchain != nullptr)
    {
        m_context->m_gui.reset();
        m_context->m_graphics_pipeline.reset();
        m_context->m_framebuffers.reset();
        m_context->m_render_pass.reset();
        m_context->m_depth_buffer.reset();
        m_context->m_swapchain.reset();
    }

    m_context->m_swapchain = std::make_unique<Swapchain>(*m_context->m_device);

    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_depth_buffer()
{
    m_context->m_depth_buffer = std::make_unique<DepthBuffer>(*m_context->m_device, *m_context->m_swapchain);
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_render_pass()
{
    m_context->m_render_pass =
        std::make_unique<RenderPass>(*m_context->m_device, *m_context->m_swapchain, *m_context->m_depth_buffer);

    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_framebuffers()
{
    m_context->m_framebuffers = std::make_unique<Framebuffers>(
        *m_context->m_device,
        *m_context->m_swapchain,
        *m_context->m_render_pass,
        *m_context->m_depth_buffer);

    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_graphics_pipeline(std::string_view layout_name)
{
    m_context->m_graphics_pipeline = std::make_unique<GraphicsPipeline>(
        *m_context->m_device,
        m_context->descriptor_set_layout(layout_name),
        *m_context->m_render_pass,
        m_context->m_render_scene->vertex_shader(),
        m_context->m_render_scene->fragment_shader());

    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::add_gui()
{
    m_context->m_gui = std::make_unique<Gui>(*m_context->m_window, *m_context->m_device, *m_context->m_render_pass);

    return *this;
}

std::unique_ptr<VulkanContext> VulkanContextBuilder::build()
{
    return std::move(m_context);
}

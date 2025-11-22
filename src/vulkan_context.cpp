#include "vulkan_context.h"

#include <stdexcept>
#include <string>

using namespace steeplejack;

VulkanContext::VulkanContext() = default;
VulkanContext::~VulkanContext() = default;

Window& VulkanContext::window()
{
    return *m_window;
}

const Device& VulkanContext::device() const
{
    return *m_device;
}

const AdhocQueues& VulkanContext::adhoc_queues() const
{
    return *m_adhoc_queues;
}

GraphicsQueue& VulkanContext::graphics_queue()
{
    return *m_graphics_queue;
}

const GraphicsQueue& VulkanContext::graphics_queue() const
{
    return *m_graphics_queue;
}

DescriptorSetLayout& VulkanContext::descriptor_set_layout(std::string_view name)
{
    auto it = m_descriptor_set_layouts.find(std::string(name));
    if (it == m_descriptor_set_layouts.end())
    {
        throw std::runtime_error("Descriptor set layout not found: " + std::string(name));
    }
    return *it->second;
}

const DescriptorSetLayout& VulkanContext::descriptor_set_layout(std::string_view name) const
{
    auto it = m_descriptor_set_layouts.find(std::string(name));
    if (it == m_descriptor_set_layouts.end())
    {
        throw std::runtime_error("Descriptor set layout not found: " + std::string(name));
    }
    return *it->second;
}

const GraphicsBuffers& VulkanContext::graphics_buffers() const
{
    return *m_graphics_buffers;
}

GraphicsBuffers& VulkanContext::graphics_buffers()
{
    return *m_graphics_buffers;
}

const Sampler& VulkanContext::sampler() const
{
    return *m_sampler;
}

const ShadowSampler& VulkanContext::shadow_sampler() const
{
    return *m_shadow_sampler;
}

TextureFactory& VulkanContext::texture_factory()
{
    return *m_texture_factory;
}

MaterialFactory& VulkanContext::material_factory()
{
    return *m_material_factory;
}

ShadowMapArray& VulkanContext::shadow_mapping()
{
    return *m_shadow_mapping;
}

ShadowFramebuffers& VulkanContext::shadow_framebuffers()
{
    return *m_shadow_framebuffers;
}

ShadowRenderPass& VulkanContext::shadow_render_pass()
{
    return *m_shadow_render_pass;
}

ShadowPipeline& VulkanContext::shadow_pipeline()
{
    return *m_shadow_pipeline;
}

const RenderScene& VulkanContext::render_scene() const
{
    return *m_render_scene;
}

RenderScene& VulkanContext::render_scene()
{
    return *m_render_scene;
}

const Swapchain& VulkanContext::swapchain() const
{
    return *m_swapchain;
}

const RenderPass& VulkanContext::render_pass() const
{
    return *m_render_pass;
}

const Framebuffers& VulkanContext::framebuffers() const
{
    return *m_framebuffers;
}

const GraphicsPipeline& VulkanContext::graphics_pipeline() const
{
    return *m_graphics_pipeline;
}

GraphicsPipeline& VulkanContext::graphics_pipeline()
{
    return *m_graphics_pipeline;
}

const Gui& VulkanContext::gui() const
{
    return *m_gui;
}

Gui& VulkanContext::gui()
{
    return *m_gui;
}

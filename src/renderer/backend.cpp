#include "renderer/backend.h"

#include "vulkan/adhoc_queues.h"
#include "vulkan/depth_buffer.h"
#include "vulkan/device.h"
#include "vulkan/framebuffers.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_queue.h"
#include "vulkan/material_factory.h"
#include "vulkan/render_pass.h"
#include "vulkan/sampler.h"
#include "vulkan/shadow_framebuffers.h"
#include "vulkan/shadow_map_array.h"
#include "vulkan/shadow_render_pass.h"
#include "vulkan/shadow_sampler.h"
#include "vulkan/swapchain.h"
#include "vulkan/texture_factory.h"
#include "vulkan/window.h"

using namespace steeplejack;
using namespace steeplejack::renderer;

Backend::Backend() = default;
Backend::~Backend() = default;

Window& Backend::window()
{
    return *m_window;
}

const Window& Backend::window() const
{
    return *m_window;
}

const Device& Backend::device() const
{
    return *m_device;
}

const AdhocQueues& Backend::adhoc_queues() const
{
    return *m_adhoc_queues;
}

GraphicsQueue& Backend::graphics_queue()
{
    return *m_graphics_queue;
}

const GraphicsQueue& Backend::graphics_queue() const
{
    return *m_graphics_queue;
}

GraphicsBuffers& Backend::graphics_buffers()
{
    return *m_graphics_buffers;
}

const GraphicsBuffers& Backend::graphics_buffers() const
{
    return *m_graphics_buffers;
}

const Sampler& Backend::sampler() const
{
    return *m_sampler;
}

const ShadowSampler& Backend::shadow_sampler() const
{
    return *m_shadow_sampler;
}

TextureFactory& Backend::texture_factory()
{
    return *m_texture_factory;
}

MaterialFactory& Backend::material_factory()
{
    return *m_material_factory;
}

ShadowMapArray& Backend::shadow_mapping()
{
    return *m_shadow_mapping;
}

ShadowFramebuffers& Backend::shadow_framebuffers()
{
    return *m_shadow_framebuffers;
}

ShadowRenderPass& Backend::shadow_render_pass()
{
    return *m_shadow_render_pass;
}

Swapchain& Backend::swapchain()
{
    return *m_swapchain;
}

const Swapchain& Backend::swapchain() const
{
    return *m_swapchain;
}

RenderPass& Backend::render_pass()
{
    return *m_render_pass;
}

const RenderPass& Backend::render_pass() const
{
    return *m_render_pass;
}

Framebuffers& Backend::framebuffers()
{
    return *m_framebuffers;
}

const Framebuffers& Backend::framebuffers() const
{
    return *m_framebuffers;
}

DepthBuffer& Backend::depth_buffer()
{
    return *m_depth_buffer;
}

const DepthBuffer& Backend::depth_buffer() const
{
    return *m_depth_buffer;
}

void Backend::rebuild_swapchain()
{
    m_window->wait_resize();
    m_device->wait_idle();

    m_swapchain = std::make_unique<Swapchain>(*m_device);
    m_depth_buffer = std::make_unique<DepthBuffer>(*m_device, *m_swapchain);
    m_render_pass = std::make_unique<RenderPass>(*m_device, *m_swapchain, *m_depth_buffer);
    m_framebuffers = std::make_unique<Framebuffers>(*m_device, *m_swapchain, *m_render_pass, *m_depth_buffer);
}

#include "renderer/backend_builder.h"

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

BackendBuilder& BackendBuilder::set_window(int width, int height, std::string title)
{
    m_config.window_width = width;
    m_config.window_height = height;
    m_config.window_title = std::move(title);
    return *this;
}

BackendBuilder& BackendBuilder::set_validation(bool enable)
{
    m_config.enable_validation = enable;
    return *this;
}

std::unique_ptr<Backend> BackendBuilder::build()
{
    auto backend = std::make_unique<Backend>();

    backend->m_window = std::make_unique<Window>(m_config.window_width, m_config.window_height, m_config.window_title);
    backend->m_device = std::make_unique<Device>(*backend->m_window, m_config.enable_validation);
    backend->m_adhoc_queues = std::make_unique<AdhocQueues>(*backend->m_device);
    backend->m_graphics_queue = std::make_unique<GraphicsQueue>(*backend->m_device);

    backend->m_graphics_buffers =
        std::make_unique<GraphicsBuffers>(*backend->m_device, *backend->m_adhoc_queues);
    backend->m_sampler = std::make_unique<Sampler>(*backend->m_device);
    backend->m_shadow_sampler = std::make_unique<ShadowSampler>(*backend->m_device);
    backend->m_texture_factory =
        std::make_unique<TextureFactory>(*backend->m_device, *backend->m_sampler, *backend->m_adhoc_queues);
    backend->m_material_factory =
        std::make_unique<MaterialFactory>(*backend->m_device, *backend->m_texture_factory);

    backend->m_shadow_mapping = std::make_unique<ShadowMapArray>(*backend->m_device, *backend->m_adhoc_queues, 8);
    backend->m_shadow_render_pass = std::make_unique<ShadowRenderPass>(*backend->m_device);
    backend->m_shadow_framebuffers = std::make_unique<ShadowFramebuffers>(
        *backend->m_device, *backend->m_shadow_mapping, *backend->m_shadow_render_pass);

    backend->m_swapchain = std::make_unique<Swapchain>(*backend->m_device);
    backend->m_depth_buffer = std::make_unique<DepthBuffer>(*backend->m_device, *backend->m_swapchain);
    backend->m_render_pass =
        std::make_unique<RenderPass>(*backend->m_device, *backend->m_swapchain, *backend->m_depth_buffer);
    backend->m_framebuffers = std::make_unique<Framebuffers>(
        *backend->m_device, *backend->m_swapchain, *backend->m_render_pass, *backend->m_depth_buffer);

    return backend;
}
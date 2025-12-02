#pragma once

#include <memory>
#include "vulkan/window.h"
#include "vulkan/device.h"
#include "vulkan/adhoc_queues.h"
#include "vulkan/graphics_queue.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/sampler.h"
#include "vulkan/shadow_sampler.h"
#include "vulkan/texture_factory.h"
#include "vulkan/material_factory.h"
#include "vulkan/shadow_map_array.h"
#include "vulkan/shadow_framebuffers.h"
#include "vulkan/shadow_render_pass.h"
#include "vulkan/swapchain.h"
#include "vulkan/depth_buffer.h"
#include "vulkan/render_pass.h"
#include "vulkan/framebuffers.h"


namespace steeplejack
{
namespace renderer
{
// Global render backend: owns core Vulkan objects for the ECS path.
class Backend
{
  private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Device> m_device;
    std::unique_ptr<AdhocQueues> m_adhoc_queues;
    std::unique_ptr<GraphicsQueue> m_graphics_queue;
    std::unique_ptr<GraphicsBuffers> m_graphics_buffers;
    std::unique_ptr<Sampler> m_sampler;
    std::unique_ptr<ShadowSampler> m_shadow_sampler;
    std::unique_ptr<TextureFactory> m_texture_factory;
    std::unique_ptr<MaterialFactory> m_material_factory;
    std::unique_ptr<ShadowMapArray> m_shadow_mapping;
    std::unique_ptr<ShadowFramebuffers> m_shadow_framebuffers;
    std::unique_ptr<ShadowRenderPass> m_shadow_render_pass;
    std::unique_ptr<Swapchain> m_swapchain;
    std::unique_ptr<DepthBuffer> m_depth_buffer;
    std::unique_ptr<RenderPass> m_render_pass;
    std::unique_ptr<Framebuffers> m_framebuffers;

  public:
    Backend();
    ~Backend();

    Backend(const Backend&) = delete;
    Backend& operator=(const Backend&) = delete;
    Backend(Backend&&) = delete;
    Backend& operator=(Backend&&) = delete;

    [[nodiscard]] Window& window();
    [[nodiscard]] const Window& window() const;

    [[nodiscard]] const Device& device() const;
    [[nodiscard]] const AdhocQueues& adhoc_queues() const;
    [[nodiscard]] GraphicsQueue& graphics_queue();
    [[nodiscard]] const GraphicsQueue& graphics_queue() const;

    [[nodiscard]] GraphicsBuffers& graphics_buffers();
    [[nodiscard]] const GraphicsBuffers& graphics_buffers() const;

    [[nodiscard]] const Sampler& sampler() const;
    [[nodiscard]] const ShadowSampler& shadow_sampler() const;

    [[nodiscard]] TextureFactory& texture_factory();
    [[nodiscard]] MaterialFactory& material_factory();

    [[nodiscard]] ShadowMapArray& shadow_mapping();
    [[nodiscard]] ShadowFramebuffers& shadow_framebuffers();
    [[nodiscard]] ShadowRenderPass& shadow_render_pass();

    [[nodiscard]] Swapchain& swapchain();
    [[nodiscard]] const Swapchain& swapchain() const;
    [[nodiscard]] RenderPass& render_pass();
    [[nodiscard]] const RenderPass& render_pass() const;
    [[nodiscard]] Framebuffers& framebuffers();
    [[nodiscard]] const Framebuffers& framebuffers() const;

    [[nodiscard]] DepthBuffer& depth_buffer();
    [[nodiscard]] const DepthBuffer& depth_buffer() const;

    void rebuild_swapchain();

    friend class BackendBuilder;
};
} // namespace renderer
} // namespace steeplejack

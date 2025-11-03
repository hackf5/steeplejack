#pragma once

#include "gui/gui.h"
#include "model/scene.h"
#include "scenes/render_scene.h"
#include "util/no_copy_or_move.h"
#include "vulkan/adhoc_queues.h"
#include "vulkan/depth_buffer.h"
#include "vulkan/descriptor_set_layout.h"
#include "vulkan/device.h"
#include "vulkan/framebuffers.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/graphics_queue.h"
#include "vulkan/material_factory.h"
#include "vulkan/render_pass.h"
#include "vulkan/sampler.h"
#include "vulkan/shadow_framebuffers.h"
#include "vulkan/shadow_map_array.h"
#include "vulkan/shadow_pipeline.h"
#include "vulkan/shadow_render_pass.h"
#include "vulkan/swapchain.h"
#include "vulkan/texture_factory.h"
#include "vulkan/vertex.h"
#include "vulkan/window.h"

#include <memory>

namespace steeplejack
{
class VulkanContext : NoCopyOrMove
{
  private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Device> m_device;
    std::unique_ptr<AdhocQueues> m_adhoc_queues;
    std::unique_ptr<GraphicsQueue> m_graphics_queue;
    std::unique_ptr<DescriptorSetLayout> m_descriptor_set_layout;
    std::unique_ptr<GraphicsBuffers> m_graphics_buffers;
    std::unique_ptr<Sampler> m_sampler;
    std::unique_ptr<TextureFactory> m_texture_factory;
    std::unique_ptr<MaterialFactory> m_material_factory;
    std::unique_ptr<ShadowMapArray> m_shadow_mapping;
    std::unique_ptr<ShadowFramebuffers> m_shadow_framebuffers;
    std::unique_ptr<ShadowRenderPass> m_shadow_render_pass;
    std::unique_ptr<ShadowPipeline> m_shadow_pipeline;
    std::unique_ptr<RenderScene> m_render_scene;
    std::unique_ptr<Swapchain> m_swapchain;
    std::unique_ptr<DepthBuffer> m_depth_buffer;
    std::unique_ptr<RenderPass> m_render_pass;
    std::unique_ptr<Framebuffers> m_framebuffers;
    std::unique_ptr<GraphicsPipeline> m_graphics_pipeline;
    std::unique_ptr<Gui> m_gui;

  public:
    VulkanContext() = default;

    Window& window()
    {
        return *m_window;
    }

    const Device& device() const
    {
        return *m_device;
    }

    const AdhocQueues& adhoc_queues() const
    {
        return *m_adhoc_queues;
    }

    GraphicsQueue& graphics_queue()
    {
        return *m_graphics_queue;
    }
    const GraphicsQueue& graphics_queue() const
    {
        return *m_graphics_queue;
    }

    const DescriptorSetLayout& descriptor_set_layout() const
    {
        return *m_descriptor_set_layout;
    }
    DescriptorSetLayout& descriptor_set_layout()
    {
        return *m_descriptor_set_layout;
    }

    const GraphicsBuffers& graphics_buffers() const
    {
        return *m_graphics_buffers;
    }
    GraphicsBuffers& graphics_buffers()
    {
        return *m_graphics_buffers;
    }

    const Sampler& sampler() const
    {
        return *m_sampler;
    }

    TextureFactory& texture_factory()
    {
        return *m_texture_factory;
    }

    MaterialFactory& material_factory()
    {
        return *m_material_factory;
    }

    ShadowMapArray& shadow_mapping()
    {
        return *m_shadow_mapping;
    }

    ShadowFramebuffers& shadow_framebuffers()
    {
        return *m_shadow_framebuffers;
    }

    ShadowRenderPass& shadow_render_pass()
    {
        return *m_shadow_render_pass;
    }

    ShadowPipeline& shadow_pipeline()
    {
        return *m_shadow_pipeline;
    }

    const RenderScene& render_scene() const
    {
        return *m_render_scene;
    }
    RenderScene& render_scene()
    {
        return *m_render_scene;
    }

    const Swapchain& swapchain() const
    {
        return *m_swapchain;
    }

    const RenderPass& render_pass() const
    {
        return *m_render_pass;
    }

    const Framebuffers& framebuffers() const
    {
        return *m_framebuffers;
    }

    const GraphicsPipeline& graphics_pipeline() const
    {
        return *m_graphics_pipeline;
    }
    GraphicsPipeline& graphics_pipeline()
    {
        return *m_graphics_pipeline;
    }

    const Gui& gui() const
    {
        return *m_gui;
    }
    Gui& gui()
    {
        return *m_gui;
    }

    friend class VulkanContextBuilder;
};
} // namespace steeplejack

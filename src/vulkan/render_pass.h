#pragma once

#include "depth_buffer.h"
#include "device.h"
#include "render_pass_scope.h"

#include <cassert>

namespace steeplejack
{
class RenderPass
{
  private:
    const Device& m_device;
    const Swapchain& m_swapchain;

    const VkRenderPass m_render_pass;

    [[nodiscard]] VkRenderPass create_render_pass(const DepthBuffer& depth_buffer) const;

  public:
    RenderPass(const Device& device, const Swapchain& swapchain, const DepthBuffer& depth_buffer);
    ~RenderPass();

    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass(RenderPass&&) = delete;
    RenderPass& operator=(RenderPass&&) = delete;

    [[nodiscard]] VkRenderPass vk() const
    {
        return m_render_pass;
    }

    RenderPassScope begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer) const;
};
} // namespace steeplejack

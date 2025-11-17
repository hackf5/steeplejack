#pragma once

#include "depth_buffer.h"
#include "device.h"
#include "swapchain.h"

namespace steeplejack
{
class RenderPass
{
  private:
    const Device& m_device;
    const Swapchain& m_swapchain;

    const VkRenderPass m_render_pass;

    VkRenderPass create_render_pass(const DepthBuffer& depth_buffer) const;

  public:
    RenderPass(const Device& device, const Swapchain& swapchain, const DepthBuffer& depth_buffer);
    ~RenderPass();

    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass(RenderPass&&) = delete;
    RenderPass& operator=(RenderPass&&) = delete;

    operator VkRenderPass() const
    {
        return m_render_pass;
    }

    void begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer) const;

    void end(VkCommandBuffer command_buffer) const
    {
        vkCmdEndRenderPass(command_buffer);
    }
};
} // namespace steeplejack

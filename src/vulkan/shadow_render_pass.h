#pragma once

#include "device.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowRenderPass
{
  private:
    const Device& m_device;
    const VkRenderPass m_render_pass;

    VkRenderPass create_render_pass(VkFormat depth_format) const;

  public:
    ShadowRenderPass(const Device& device, VkFormat depth_format = VK_FORMAT_D32_SFLOAT);
    ~ShadowRenderPass();

    ShadowRenderPass(const ShadowRenderPass&) = delete;
    ShadowRenderPass& operator=(const ShadowRenderPass&) = delete;
    ShadowRenderPass(ShadowRenderPass&&) = delete;
    ShadowRenderPass& operator=(ShadowRenderPass&&) = delete;

    operator VkRenderPass() const
    {
        return m_render_pass;
    }

    void begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, uint32_t resolution) const;

    void end(VkCommandBuffer command_buffer) const
    {
        vkCmdEndRenderPass(command_buffer);
    }
};
} // namespace steeplejack

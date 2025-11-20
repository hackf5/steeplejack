#pragma once

#include "device.h"
#include "render_pass_scope.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowRenderPass
{
  private:
    const Device& m_device;
    const VkRenderPass m_render_pass;

  public:
    explicit ShadowRenderPass(const Device& device, VkFormat depth_format = VK_FORMAT_D32_SFLOAT);
    ~ShadowRenderPass();

    ShadowRenderPass(const ShadowRenderPass&) = delete;
    ShadowRenderPass& operator=(const ShadowRenderPass&) = delete;
    ShadowRenderPass(ShadowRenderPass&&) = delete;
    ShadowRenderPass& operator=(ShadowRenderPass&&) = delete;

    [[nodiscard]] VkRenderPass vk() const;

    RenderPassScope begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, uint32_t resolution) const;
};
} // namespace steeplejack

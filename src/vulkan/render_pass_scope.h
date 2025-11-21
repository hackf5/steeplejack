#pragma once

#include <vulkan/vulkan.h>

namespace steeplejack
{
class [[nodiscard]] RenderPassScope
{
  public:
    RenderPassScope(const RenderPassScope&) = delete;
    RenderPassScope& operator=(const RenderPassScope&) = delete;
    RenderPassScope& operator=(RenderPassScope&& other) = delete;
    RenderPassScope(RenderPassScope&& other) noexcept;
    ~RenderPassScope();

  private:
    explicit RenderPassScope(VkCommandBuffer command_buffer);

    VkCommandBuffer m_command_buffer = nullptr;

    friend class RenderPass;
    friend class ShadowRenderPass;
};
} // namespace steeplejack

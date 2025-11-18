#pragma once

#include <cassert>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class [[nodiscard]] RenderPassScope
{
  public:
    RenderPassScope(const RenderPassScope&) = delete;
    RenderPassScope& operator=(const RenderPassScope&) = delete;
    RenderPassScope& operator=(RenderPassScope&& other) = delete;
    RenderPassScope(RenderPassScope&& other) noexcept : m_command_buffer(other.m_command_buffer)
    {
        assert(other.m_command_buffer != nullptr);
        other.m_command_buffer = nullptr;
    }
    ~RenderPassScope()
    {
        if (m_command_buffer != nullptr)
        {
            vkCmdEndRenderPass(m_command_buffer);
            m_command_buffer = nullptr;
        }
    }

  private:
    explicit RenderPassScope(VkCommandBuffer command_buffer) : m_command_buffer(command_buffer) {}

    VkCommandBuffer m_command_buffer = nullptr;

    friend class RenderPass;
    friend class ShadowRenderPass;
};
} // namespace steeplejack
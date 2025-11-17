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

    [[nodiscard]] VkRenderPass create_render_pass(VkFormat depth_format) const;

  public:
    class [[nodiscard]] Scope
    {
      public:
        Scope(const Scope&) = delete;
        Scope& operator=(const Scope&) = delete;
        Scope(Scope&& other) noexcept : m_command_buffer(other.m_command_buffer)
        {
            other.m_command_buffer = nullptr;
        }
        Scope& operator=(Scope&& other) noexcept
        {
            if (this != &other)
            {
                end();
                m_command_buffer = other.m_command_buffer;
                other.m_command_buffer = nullptr;
            }
            return *this;
        }
        ~Scope()
        {
            end();
        }

      private:
        explicit Scope(VkCommandBuffer command_buffer) : m_command_buffer(command_buffer) {}

        VkCommandBuffer m_command_buffer = nullptr;

        void end()
        {
            if (m_command_buffer != nullptr)
            {
                vkCmdEndRenderPass(m_command_buffer);
                m_command_buffer = nullptr;
            }
        }

        friend class ShadowRenderPass;
    };

    explicit ShadowRenderPass(const Device& device, VkFormat depth_format = VK_FORMAT_D32_SFLOAT);
    ~ShadowRenderPass();

    ShadowRenderPass(const ShadowRenderPass&) = delete;
    ShadowRenderPass& operator=(const ShadowRenderPass&) = delete;
    ShadowRenderPass(ShadowRenderPass&&) = delete;
    ShadowRenderPass& operator=(ShadowRenderPass&&) = delete;

    [[nodiscard]] VkRenderPass vk() const
    {
        return m_render_pass;
    }

    Scope begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, uint32_t resolution) const;
};
} // namespace steeplejack

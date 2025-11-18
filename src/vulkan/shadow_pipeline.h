#pragma once

#include "descriptor_set_layout.h"
#include "device.h"
#include "shadow_render_pass.h"

#include <memory>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowPipeline
{
  private:
    const Device& m_device;
    const std::unique_ptr<DescriptorSetLayout> m_descriptor_set_layout;
    const VkPipelineLayout m_pipeline_layout;
    const VkPipeline m_pipeline;

    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR; // NOLINT(readability-identifier-naming)

    [[nodiscard]] std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout() const;
    [[nodiscard]] VkPipeline create_pipeline(
        const ShadowRenderPass& shadow_render_pass,
        const std::string& vertex_shader,
        const std::string& fragment_shader) const;

    [[nodiscard]] PFN_vkCmdPushDescriptorSetKHR fetch_vkCmdPushDescriptorSetKHR(); // NOLINT(readability-identifier-naming)

  public:
    ShadowPipeline(
        const Device& device,
        const ShadowRenderPass& shadow_render_pass,
        const std::string& vertex_shader,
        const std::string& fragment_shader);
    ~ShadowPipeline();

    ShadowPipeline(const ShadowPipeline&) = delete;
    ShadowPipeline& operator=(const ShadowPipeline&) = delete;
    ShadowPipeline(ShadowPipeline&&) = delete;
    ShadowPipeline& operator=(ShadowPipeline&&) = delete;

    [[nodiscard]] DescriptorSetLayout& descriptor_set_layout() const
    {
        return *m_descriptor_set_layout;
    }

    void bind(VkCommandBuffer command_buffer) const
    {
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    }

    void push_descriptor_set(VkCommandBuffer command_buffer) const
    {
        auto write_descriptor_sets = m_descriptor_set_layout->get_write_descriptor_sets();
        vkCmdPushDescriptorSetKHR(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout,
            0,
            write_descriptor_sets.size(),
            write_descriptor_sets.data());
    }
};
} // namespace steeplejack

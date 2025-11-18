#pragma once

#include "descriptor_set_layout.h"
#include "device.h"
#include "render_pass.h"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class GraphicsPipeline
{
  private:
    const Device& m_device;
    DescriptorSetLayout& m_descriptor_set_layout;

    const VkPipelineLayout m_pipeline_layout;
    const VkPipeline m_pipeline;

    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR; // NOLINT(readability-identifier-naming)

    VkPipeline create_pipeline(
        const RenderPass& render_pass, const std::string& vertex_shader, const std::string& fragment_shader);

    PFN_vkCmdPushDescriptorSetKHR fetch_vkCmdPushDescriptorSetKHR(); // NOLINT(readability-identifier-naming)

  public:
    GraphicsPipeline(
        const Device& device,
        DescriptorSetLayout& descriptor_set_layout,
        const RenderPass& render_pass,
        const std::string& vertex_shader,
        const std::string& fragment_shader);
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&&) = delete;
    GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;

    [[nodiscard]] VkPipelineLayout layout() const
    {
        return m_pipeline_layout;
    }

    [[nodiscard]] DescriptorSetLayout& descriptor_set_layout() const
    {
        return m_descriptor_set_layout;
    }

    void bind(VkCommandBuffer command_buffer) const
    {
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    }

    void push_descriptor_set(VkCommandBuffer command_buffer) const
    {
        auto write_descriptor_sets = m_descriptor_set_layout.get_write_descriptor_sets();
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

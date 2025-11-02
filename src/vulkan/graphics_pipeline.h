#pragma once

#include "descriptor_set_layout.h"
#include "device.h"
#include "render_pass.h"
#include "shader_module.h"
#include "swapchain.h"
#include "util/no_copy_or_move.h"

#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class GraphicsPipeline : NoCopyOrMove
{
  private:
    const Device& m_device;
    DescriptorSetLayout& m_descriptor_set_layout;

    const VkPipelineLayout m_pipeline_layout;
    const VkPipeline m_pipeline;

    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;

    VkPipeline create_pipeline(
        const RenderPass& render_pass, const std::string& vertex_shader, const std::string& fragment_shader);

    PFN_vkCmdPushDescriptorSetKHR fetch_vkCmdPushDescriptorSetKHR();

  public:
    GraphicsPipeline(
        const Device& device,
        DescriptorSetLayout& descriptor_set_layout,
        const RenderPass& render_pass,
        const std::string& vertex_shader,
        const std::string& fragment_shader);
    ~GraphicsPipeline();

    operator VkPipeline() const
    {
        return m_pipeline;
    }

    VkPipelineLayout layout() const
    {
        return m_pipeline_layout;
    }

    DescriptorSetLayout& descriptor_set_layout() const
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
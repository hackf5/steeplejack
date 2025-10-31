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
        const Swapchain& swapchain,
        const RenderPass& render_pass,
        const std::string& vertex_shader,
        const std::string& fragment_shader);

    static std::vector<VkPipelineShaderStageCreateInfo>
    create_shader_stages(const ShaderModule& vertex_shader, const ShaderModule& fragment_shader);

    static VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state();

    static VkPipelineViewportStateCreateInfo create_viewport_state(const Swapchain& swapchain);

    static VkPipelineRasterizationStateCreateInfo create_rasterization_state();

    VkPipelineMultisampleStateCreateInfo create_multisample_state();

    static VkPipelineColorBlendAttachmentState create_color_blend_attachment_state();

    static VkPipelineColorBlendStateCreateInfo
    create_color_blend_state(const VkPipelineColorBlendAttachmentState& color_blend_attachment_state);

    static std::vector<VkDynamicState> create_dynamic_states();

    static VkPipelineDynamicStateCreateInfo create_dynamic_state(const std::vector<VkDynamicState>& dynamic_states);

    static VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state();

    PFN_vkCmdPushDescriptorSetKHR fetch_vkCmdPushDescriptorSetKHR();

  public:
    GraphicsPipeline(
        const Device& device,
        DescriptorSetLayout& descriptor_set_layout,
        const Swapchain& swapchain,
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
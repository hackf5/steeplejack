#pragma once

#include "descriptor_set_layout.h"
#include "device.h"
#include "render_pass.h"

#include <string>
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

    [[nodiscard]] VkPipelineLayout layout() const;

    [[nodiscard]] DescriptorSetLayout& descriptor_set_layout() const;

    void bind(VkCommandBuffer command_buffer) const;

    void push_descriptor_set(VkCommandBuffer command_buffer) const;
};
} // namespace steeplejack

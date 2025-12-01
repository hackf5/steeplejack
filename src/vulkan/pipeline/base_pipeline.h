// NOLINTBEGIN

#pragma once

#include "config/pipeline_config.h"
#include "vulkan/descriptor_set_layout.h"
#include "vulkan/device.h"
#include "vulkan/render_pass.h"

#include <functional>
#include <string>
#include <vulkan/vulkan.h>

namespace steeplejack::pipeline
{
// Common base for graphics/shadow pipelines.
class BasePipeline
{
  private:
    const Device& m_device;
    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR{nullptr}; // NOLINT(readability-identifier-naming)

    config::PipelineDefinition m_pipeline_definition;
    DescriptorSetLayout m_descriptor_set_layout;
    VkPipelineLayout m_pipeline_layout{nullptr};
    VkPipeline m_pipeline{nullptr};

  protected:
    using create_pipeline_func =
        std::function<VkPipeline(const Device&, const config::PipelineDefinition&, const RenderPass&)>;

    BasePipeline(
        const Device& device,
        const RenderPass& render_pass,
        const std::string& pipeline_name,
        const create_pipeline_func& create_pipeline);

  public:
    virtual ~BasePipeline();

    BasePipeline(const BasePipeline&) = delete;
    BasePipeline& operator=(const BasePipeline&) = delete;
    BasePipeline(BasePipeline&&) = delete;
    BasePipeline& operator=(BasePipeline&&) = delete;

    [[nodiscard]] VkPipelineLayout layout() const;

    [[nodiscard]] DescriptorSetLayout& descriptor_set_layout();
    [[nodiscard]] const DescriptorSetLayout& descriptor_set_layout() const;

    void bind(VkCommandBuffer command_buffer) const;

    void push_descriptor_set(VkCommandBuffer command_buffer) const;
};
} // namespace steeplejack::pipeline

// NOLINTEND

#pragma once

#include "descriptor_set_layout.h"
#include "device.h"
#include "shadow_render_pass.h"

#include <string_view>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowPipeline
{
  private:
    const Device& m_device;
    DescriptorSetLayout& m_descriptor_set_layout;
    const VkPipelineLayout m_pipeline_layout;
    const VkPipeline m_pipeline;

    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR; // NOLINT(readability-identifier-naming)

  public:
    ShadowPipeline(
        const Device& device,
        DescriptorSetLayout& descriptor_set_layout,
        const ShadowRenderPass& shadow_render_pass,
        std::string_view vertex_shader,
        std::string_view fragment_shader);
    ~ShadowPipeline();

    ShadowPipeline(const ShadowPipeline&) = delete;
    ShadowPipeline& operator=(const ShadowPipeline&) = delete;
    ShadowPipeline(ShadowPipeline&&) = delete;
    ShadowPipeline& operator=(ShadowPipeline&&) = delete;

    [[nodiscard]] DescriptorSetLayout& descriptor_set_layout() const;

    void bind(VkCommandBuffer command_buffer) const;

    void push_descriptor_set(VkCommandBuffer command_buffer) const;
};
} // namespace steeplejack

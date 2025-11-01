#pragma once

#include "shader_module.h"

#include <array>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
namespace pipeline
{
std::vector<VkPipelineShaderStageCreateInfo>
create_shader_stages(const ShaderModule& vertex_shader, const ShaderModule& fragment_shader);

VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state();

VkPipelineViewportStateCreateInfo create_dynamic_viewport_state(const VkViewport& viewport, const VkRect2D& scissor);

VkPipelineRasterizationStateCreateInfo create_rasterization_state();

VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state();

VkPipelineColorBlendStateCreateInfo create_color_blend_state(
    const VkPipelineColorBlendAttachmentState& blend_attachment_state);

VkPipelineDynamicStateCreateInfo create_dynamic_state(const std::span<const VkDynamicState>& dynamic_states);
} // namespace pipeline
} // namespace steeplejack
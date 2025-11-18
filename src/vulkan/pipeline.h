#pragma once

#include "shader_module.h"

#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack::pipeline
{
inline constexpr VkViewport kPlaceholderViewport{.x=0, .y=0, .width=1, .height=1, .minDepth=0.0F, .maxDepth=1.0F};

inline constexpr VkRect2D kPlaceholderScissor{.offset{.x=0, .y=0}, .extent {.width=1, .height=1}};

std::vector<VkPipelineShaderStageCreateInfo>
create_shader_stages(const ShaderModule& vertex_shader, const ShaderModule& fragment_shader);

VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state();

VkPipelineViewportStateCreateInfo create_dynamic_viewport_state();

VkPipelineRasterizationStateCreateInfo create_rasterization_state();

VkPipelineRasterizationStateCreateInfo create_shadow_rasterization_state();

VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state();

VkPipelineColorBlendStateCreateInfo
create_color_blend_state(const VkPipelineColorBlendAttachmentState& blend_attachment_state);

VkPipelineDynamicStateCreateInfo create_dynamic_state(const std::span<const VkDynamicState>& dynamic_states);
} // namespace steeplejack::pipeline

#include "pipeline.h"

using namespace steeplejack;

std::vector<VkPipelineShaderStageCreateInfo>
pipeline::create_shader_stages(const ShaderModule& vertex_shader, const ShaderModule& fragment_shader)
{
    VkPipelineShaderStageCreateInfo vert_stage_info = {};
    vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_info.module = vertex_shader;
    vert_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_stage_info = {};
    frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_info.module = fragment_shader;
    frag_stage_info.pName = "main";

    return {vert_stage_info, frag_stage_info};
}

VkPipelineInputAssemblyStateCreateInfo pipeline::create_input_assembly_state()
{
    VkPipelineInputAssemblyStateCreateInfo result = {};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    result.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    result.primitiveRestartEnable = VK_FALSE;

    return result;
}

VkPipelineViewportStateCreateInfo pipeline::create_dynamic_viewport_state(const VkViewport& viewport, const VkRect2D& scissor)
{
    VkPipelineViewportStateCreateInfo result = {};

    result.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    result.viewportCount = 1;
    result.pViewports = &viewport;
    result.scissorCount = 1;
    result.pScissors = &scissor;

    return result;
}

VkPipelineRasterizationStateCreateInfo pipeline::create_rasterization_state()
{
    VkPipelineRasterizationStateCreateInfo result = {};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    result.depthClampEnable = VK_FALSE;
    result.rasterizerDiscardEnable = VK_FALSE;
    result.polygonMode = VK_POLYGON_MODE_FILL;
    result.lineWidth = 1.0F;
    result.cullMode =  VK_CULL_MODE_BACK_BIT;
    result.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    result.depthBiasEnable = VK_FALSE;

    return result;
}

VkPipelineDepthStencilStateCreateInfo pipeline::create_depth_stencil_state()
{
    VkPipelineDepthStencilStateCreateInfo result = {};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    result.depthTestEnable = VK_TRUE;
    result.depthWriteEnable = VK_TRUE;
    result.depthCompareOp = VK_COMPARE_OP_LESS;
    result.depthBoundsTestEnable = VK_FALSE;
    result.stencilTestEnable = VK_FALSE;
    result.minDepthBounds = 0.0F;
    result.maxDepthBounds = 1.0F;
    result.front = {};
    result.back = {};

    return result;
}

VkPipelineColorBlendStateCreateInfo pipeline::create_color_blend_state(const VkPipelineColorBlendAttachmentState& blend_attachment_state)
{
    VkPipelineColorBlendStateCreateInfo result = {};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    result.logicOpEnable = VK_FALSE;
    result.logicOp = VK_LOGIC_OP_COPY;
    result.attachmentCount = 1;
    result.pAttachments = &blend_attachment_state;
    result.blendConstants[0] = 0.0F;
    result.blendConstants[1] = 0.0F;
    result.blendConstants[2] = 0.0F;
    result.blendConstants[3] = 0.0F;

    return result;
}

VkPipelineDynamicStateCreateInfo pipeline::create_dynamic_state(const std::span<const VkDynamicState>& dynamic_states)
{
    VkPipelineDynamicStateCreateInfo result = {};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    result.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    result.pDynamicStates = dynamic_states.data();

    return result;
}
#include "vulkan/pipeline/shadow_pipeline.h"

#include "config/pipeline_config.h"
#include "vulkan/pipeline.h"
#include "vulkan/pipeline/base_pipeline.h"
#include "vulkan/shader_module.h"
#include "vulkan/shadow_render_pass.h"

#include <stdexcept>
#include <vector>

using namespace steeplejack;
using namespace steeplejack::pipeline;

namespace
{
VkFormat parse_vertex_format(const std::string& fmt)
{
    if (fmt == "R32G32B32_SFLOAT")
    {
        return VK_FORMAT_R32G32B32_SFLOAT;
    }
    if (fmt == "R32G32_SFLOAT")
    {
        return VK_FORMAT_R32G32_SFLOAT;
    }
    throw std::runtime_error("Unsupported vertex format: " + fmt);
}

VkVertexInputRate parse_input_rate(const std::string& rate)
{
    if (rate == "vertex")
    {
        return VK_VERTEX_INPUT_RATE_VERTEX;
    }
    if (rate == "instance")
    {
        return VK_VERTEX_INPUT_RATE_INSTANCE;
    }
    throw std::runtime_error("Unsupported input rate: " + rate);
}
} // namespace

ShadowPipeline::ShadowPipeline(
    const Device& device, const ShadowRenderPass& render_pass, const std::string& pipeline_name) :
    BasePipeline(
        device,
        pipeline_name,
        [&device, &render_pass](const config::PipelineDefinition& def, VkPipelineLayout layout)
        {
            std::vector<VkVertexInputBindingDescription> binding_descs;
            binding_descs.reserve(def.vertex_bindings.size());
            for (const auto& b : def.vertex_bindings)
            {
                VkVertexInputBindingDescription desc{};
                desc.binding = b.binding;
                desc.stride = b.stride;
                desc.inputRate = parse_input_rate(b.input_rate);
                binding_descs.push_back(desc);
            }

            std::vector<VkVertexInputAttributeDescription> attr_descs;
            attr_descs.reserve(def.vertex_attributes.size());
            for (const auto& a : def.vertex_attributes)
            {
                VkVertexInputAttributeDescription desc{};
                desc.binding = a.binding;
                desc.location = a.location;
                desc.format = parse_vertex_format(a.format);
                desc.offset = a.offset;
                attr_descs.push_back(desc);
            }

            VkPipelineVertexInputStateCreateInfo vertex_input{};
            vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertex_input.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descs.size());
            vertex_input.pVertexBindingDescriptions = binding_descs.data();
            vertex_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(attr_descs.size());
            vertex_input.pVertexAttributeDescriptions = attr_descs.data();

            auto vertex_shader_module = ShaderModule(device, def.vertex_shader);
            auto fragment_shader_module = ShaderModule(device, def.fragment_shader);
            auto shader_stages = pipeline::create_shader_stages(vertex_shader_module, fragment_shader_module);

            auto input_assembly_state = pipeline::create_input_assembly_state();
            auto viewport_state = pipeline::create_dynamic_viewport_state();
            auto rasterization_state = pipeline::create_shadow_rasterization_state();

            auto dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_BIAS};
            auto dynamic_state = pipeline::create_dynamic_state(dynamic_states);

            auto depth_stencil_state = pipeline::create_depth_stencil_state();

            VkPipelineMultisampleStateCreateInfo multisample_state = {};
            multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisample_state.sampleShadingEnable = VK_FALSE;

            VkGraphicsPipelineCreateInfo pipeline_info = {};
            pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
            pipeline_info.pStages = shader_stages.data();
            pipeline_info.pVertexInputState = &vertex_input;
            pipeline_info.pInputAssemblyState = &input_assembly_state;
            pipeline_info.pViewportState = &viewport_state;
            pipeline_info.pRasterizationState = &rasterization_state;
            pipeline_info.pMultisampleState = &multisample_state;
            pipeline_info.pColorBlendState = nullptr;
            pipeline_info.pDynamicState = &dynamic_state;
            pipeline_info.pDepthStencilState = &depth_stencil_state;
            pipeline_info.layout = layout;
            pipeline_info.renderPass = render_pass.vk();
            pipeline_info.subpass = 0;
            pipeline_info.basePipelineHandle = nullptr;

            VkPipeline pipeline = VK_NULL_HANDLE;
            if (vkCreateGraphicsPipelines(device.vk(), nullptr, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create shadow pipeline");
            }
            return pipeline;
        })
{
}

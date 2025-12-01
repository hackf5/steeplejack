#include "vulkan/pipeline/graphics_pipeline.h"

#include "config/pipeline_config.h"
#include "vulkan/pipeline/utils.h"
#include "vulkan/shader_module.h"

#include <stdexcept>
#include <vector>

using namespace steeplejack;
using namespace steeplejack::pipeline;

GraphicsPipeline::GraphicsPipeline(
    const Device& device, const RenderPass& render_pass, const std::string& pipeline_name) :
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
            auto rasterization_state = pipeline::create_rasterization_state();

            VkPipelineMultisampleStateCreateInfo multisampling_state = {};
            multisampling_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling_state.sampleShadingEnable = VK_FALSE;
            multisampling_state.rasterizationSamples = device.msaa_samples();

            VkPipelineColorBlendAttachmentState color_blend_attachment = {};
            color_blend_attachment.blendEnable = VK_FALSE;
            color_blend_attachment.colorWriteMask = static_cast<VkColorComponentFlags>(
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT);
            auto color_blend_state = pipeline::create_color_blend_state(color_blend_attachment);

            auto dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
            auto dynamic_state = pipeline::create_dynamic_state(dynamic_states);

            auto depth_stencil_state = pipeline::create_depth_stencil_state();

            VkGraphicsPipelineCreateInfo pipeline_info = {};
            pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
            pipeline_info.pStages = shader_stages.data();
            pipeline_info.pVertexInputState = &vertex_input;
            pipeline_info.pInputAssemblyState = &input_assembly_state;
            pipeline_info.pViewportState = &viewport_state;
            pipeline_info.pRasterizationState = &rasterization_state;
            pipeline_info.pMultisampleState = &multisampling_state;
            pipeline_info.pColorBlendState = &color_blend_state;
            pipeline_info.pDynamicState = &dynamic_state;
            pipeline_info.pDepthStencilState = &depth_stencil_state;
            pipeline_info.layout = layout;
            pipeline_info.renderPass = render_pass.vk();
            pipeline_info.subpass = 0;
            pipeline_info.basePipelineHandle = nullptr;

            VkPipeline pipeline = VK_NULL_HANDLE;
            if (vkCreateGraphicsPipelines(device.vk(), nullptr, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create graphics pipeline");
            }
            return pipeline;
        })
{
}
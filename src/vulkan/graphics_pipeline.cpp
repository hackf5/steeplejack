#include "graphics_pipeline.h"

#include "pipeline.h"
#include "spdlog/spdlog.h"
#include "vertex.h"

using namespace steeplejack;

GraphicsPipeline::GraphicsPipeline(
    const Device& device,
    DescriptorSetLayout& descriptor_set_layout,
    const RenderPass& render_pass,
    const std::string& vertex_shader,
    const std::string& fragment_shader) :
    m_device(device),
    m_descriptor_set_layout(descriptor_set_layout),
    m_pipeline_layout(m_descriptor_set_layout.create_pipeline_layout()),
    m_pipeline(create_pipeline(render_pass, vertex_shader, fragment_shader)),
    vkCmdPushDescriptorSetKHR(fetch_vkCmdPushDescriptorSetKHR())
{
}

GraphicsPipeline::~GraphicsPipeline()
{
    spdlog::info("Destroying Graphics Pipeline");
    vkDestroyPipeline(m_device.vk(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device.vk(), m_pipeline_layout, nullptr);
}

VkPipeline GraphicsPipeline::create_pipeline(
    const RenderPass& render_pass, const std::string& vertex_shader, const std::string& fragment_shader)
{
    spdlog::info("Creating Graphics Pipeline");

    auto vertex_input_state = VertexInputState::make_all(0);

    auto vertex_shader_module = ShaderModule(m_device, vertex_shader);
    auto fragment_shader_module = ShaderModule(m_device, fragment_shader);
    auto shader_stages = pipeline::create_shader_stages(vertex_shader_module, fragment_shader_module);

    auto input_assembly_state = pipeline::create_input_assembly_state();

    auto viewport_state = pipeline::create_dynamic_viewport_state();

    auto rasterization_state = pipeline::create_rasterization_state();

    VkPipelineMultisampleStateCreateInfo multisampling_state = {};
    multisampling_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state.sampleShadingEnable = VK_FALSE;
    multisampling_state.rasterizationSamples = m_device.msaa_samples();

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.colorWriteMask = static_cast<VkColorComponentFlags>(
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    auto color_blend_state = pipeline::create_color_blend_state(color_blend_attachment);

    auto dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    auto dynamic_state = pipeline::create_dynamic_state(dynamic_states);

    auto depth_stencil_state = pipeline::create_depth_stencil_state();

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.pVertexInputState = &vertex_input_state.pipeline;
    pipeline_info.pInputAssemblyState = &input_assembly_state;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterization_state;
    pipeline_info.pMultisampleState = &multisampling_state;
    pipeline_info.pColorBlendState = &color_blend_state;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.pDepthStencilState = &depth_stencil_state;
    pipeline_info.layout = m_pipeline_layout;
    pipeline_info.renderPass = render_pass.vk();
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = nullptr;

    VkPipeline pipeline = nullptr;
    if (vkCreateGraphicsPipelines(m_device.vk(), nullptr, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline");
    }

    return pipeline;
}

PFN_vkCmdPushDescriptorSetKHR GraphicsPipeline::fetch_vkCmdPushDescriptorSetKHR()
{
    auto result = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(
        vkGetDeviceProcAddr(m_device.vk(), "vkCmdPushDescriptorSetKHR"));
    if (result == nullptr)
    {
        throw std::runtime_error("Failed to load vkCmdPushDescriptorSetKHR");
    }

    return result;
}

#include "shadow_pipeline.h"

#include "shader_module.h"
#include "descriptor_set_layout_builder.h"
#include "pipeline.h"
#include "vertex.h"

#include "spdlog/spdlog.h"

#include <array>

using namespace steeplejack;

ShadowPipeline::ShadowPipeline(const Device& device,
                                 const ShadowRenderPass& shadow_render_pass,
                                 const std::string& vertex_shader,
                                 const std::string& fragment_shader) :
    m_device(device),
    m_descriptor_set_layout(create_descriptor_set_layout()),
    m_pipeline_layout(m_descriptor_set_layout->create_pipeline_layout()),
    m_pipeline(create_pipeline(shadow_render_pass, vertex_shader, fragment_shader))
{
}

ShadowPipeline::~ShadowPipeline()
{
    spdlog::info("Destroying Shadow Pipeline");
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
}

std::unique_ptr<DescriptorSetLayout> ShadowPipeline::create_descriptor_set_layout() const
{
    spdlog::info("Creating Shadow Pipeline Descriptor Set Layout");

    auto layout_builder = DescriptorSetLayoutBuilder();
    layout_builder.add_uniform_buffer(); // light space matrix (vertex)
    layout_builder.add_uniform_buffer(); // model matrix (vertex)
    return layout_builder.build(m_device);
}

VkPipeline ShadowPipeline::create_pipeline(
    const ShadowRenderPass& shadow_render_pass,
    const std::string& vertex_shader,
    const std::string& fragment_shader) const
{
    spdlog::info("Creating Shadow Pipeline");

    auto vertex_input_state = VertexInputState(0, Vertex::kPositionOnly);

    auto vertex_shader_module = ShaderModule(m_device, vertex_shader);
    auto fragment_shader_module = ShaderModule(m_device, fragment_shader);
    auto shader_stages = pipeline::create_shader_stages(vertex_shader_module, fragment_shader_module);

    auto input_assembly_state = pipeline::create_input_assembly_state();

    VkViewport vp{0,0,1,1,0.0f,1.0f}; VkRect2D sc{{0,0},{1,1}};
    auto viewport_state = pipeline::create_dynamic_viewport_state(vp, sc);

    // todo: enable depth bias for shadow pipeline
    auto rasterization_state = pipeline::create_rasterization_state();

    std::array<VkDynamicState, 3> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_BIAS};
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
    pipeline_info.pVertexInputState = &vertex_input_state.pipeline;
    pipeline_info.pInputAssemblyState = &input_assembly_state;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterization_state;
    pipeline_info.pMultisampleState = &multisample_state;
    pipeline_info.pColorBlendState = VK_NULL_HANDLE;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.pDepthStencilState = &depth_stencil_state;
    pipeline_info.layout = m_pipeline_layout;
    pipeline_info.renderPass = shadow_render_pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    VkPipeline pipeline = nullptr;
    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline");
    }

    return pipeline;
}

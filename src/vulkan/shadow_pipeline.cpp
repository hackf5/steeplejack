#include "shadow_pipeline.h"

#include "pipeline.h"
#include "shader_module.h"
#include "spdlog/spdlog.h"
#include "vertex.h"

using namespace steeplejack;

namespace
{
[[nodiscard]] VkPipeline create_pipeline(
    const Device& device,
    const ShadowRenderPass& shadow_render_pass,
    VkPipelineLayout pipeline_layout,
    std::string_view vertex_shader,
    std::string_view fragment_shader)
{
    auto vertex_input_state = VertexInputState::make_position_only(0);

    auto vertex_shader_module = ShaderModule(device, vertex_shader);
    auto fragment_shader_module = ShaderModule(device, fragment_shader);
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
    pipeline_info.pVertexInputState = &vertex_input_state.pipeline;
    pipeline_info.pInputAssemblyState = &input_assembly_state;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterization_state;
    pipeline_info.pMultisampleState = &multisample_state;
    pipeline_info.pColorBlendState = nullptr;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.pDepthStencilState = &depth_stencil_state;
    pipeline_info.layout = pipeline_layout;
    pipeline_info.renderPass = shadow_render_pass.vk();
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = nullptr;

    VkPipeline pipeline = nullptr;
    if (vkCreateGraphicsPipelines(device.vk(), nullptr, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline");
    }

    return pipeline;
}

[[nodiscard]] PFN_vkCmdPushDescriptorSetKHR
fetch_vkCmdPushDescriptorSetKHR(const Device& device) // NOLINT(readability-identifier-naming)
{
    auto result =
        reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(vkGetDeviceProcAddr(device.vk(), "vkCmdPushDescriptorSetKHR"));
    if (result == nullptr)
    {
        throw std::runtime_error("Failed to load vkCmdPushDescriptorSetKHR");
    }

    return result;
}
} // namespace

ShadowPipeline::ShadowPipeline(
    const Device& device,
    DescriptorSetLayout& descriptor_set_layout,
    const ShadowRenderPass& shadow_render_pass,
    std::string_view vertex_shader,
    std::string_view fragment_shader) :
    m_device(device),
    m_descriptor_set_layout(descriptor_set_layout),
    m_pipeline_layout(m_descriptor_set_layout.create_pipeline_layout()),
    m_pipeline(create_pipeline(device, shadow_render_pass, m_pipeline_layout, vertex_shader, fragment_shader)),
    vkCmdPushDescriptorSetKHR(fetch_vkCmdPushDescriptorSetKHR(device))
{
    spdlog::info("Creating Shadow Pipeline");
}

ShadowPipeline::~ShadowPipeline()
{
    spdlog::info("Destroying Shadow Pipeline");
    vkDestroyPipeline(m_device.vk(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device.vk(), m_pipeline_layout, nullptr);
}

DescriptorSetLayout& ShadowPipeline::descriptor_set_layout() const
{
    return m_descriptor_set_layout;
}

void ShadowPipeline::bind(VkCommandBuffer command_buffer) const
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}

void ShadowPipeline::push_descriptor_set(VkCommandBuffer command_buffer) const
{
    auto write_descriptor_sets = m_descriptor_set_layout.get_write_descriptor_sets();
    vkCmdPushDescriptorSetKHR(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline_layout,
        0,
        write_descriptor_sets.size(),
        write_descriptor_sets.data());
}

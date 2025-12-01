#include "vulkan/pipeline/base_pipeline.h"

#include <stdexcept>

using namespace steeplejack;
using namespace steeplejack::pipeline;

namespace
{
PFN_vkCmdPushDescriptorSetKHR
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

BasePipeline::BasePipeline(
    const Device& device, const std::string& pipeline_name, const create_pipeline_func& create_pipeline) :
    m_device(device),
    vkCmdPushDescriptorSetKHR(fetch_vkCmdPushDescriptorSetKHR(device)),
    m_pipeline_definition(config::PipelineConfig::require(pipeline_name)),
    m_descriptor_set_layout(device, m_pipeline_definition.descriptor_bindings),
    m_pipeline_layout(m_descriptor_set_layout.create_pipeline_layout()),
    m_pipeline(create_pipeline(m_pipeline_definition, m_pipeline_layout))
{
}

BasePipeline::~BasePipeline()
{
    if (m_pipeline != nullptr)
    {
        vkDestroyPipeline(m_device.vk(), m_pipeline, nullptr);
    }
    if (m_pipeline_layout != nullptr)
    {
        vkDestroyPipelineLayout(m_device.vk(), m_pipeline_layout, nullptr);
    }
}

VkPipelineLayout BasePipeline::layout() const
{
    return m_pipeline_layout;
}

DescriptorSetLayout& BasePipeline::descriptor_set_layout()
{
    return m_descriptor_set_layout;
}

const DescriptorSetLayout& BasePipeline::descriptor_set_layout() const
{
    return m_descriptor_set_layout;
}

void BasePipeline::bind(VkCommandBuffer command_buffer) const
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}

void BasePipeline::push_descriptor_set(VkCommandBuffer command_buffer) const
{
    auto write_descriptor_sets = m_descriptor_set_layout.get_write_descriptor_sets();
    vkCmdPushDescriptorSetKHR(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline_layout,
        0,
        static_cast<uint32_t>(write_descriptor_sets.size()),
        write_descriptor_sets.data());
}

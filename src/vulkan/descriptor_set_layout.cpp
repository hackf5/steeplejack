#include "descriptor_set_layout.h"

#include "spdlog/spdlog.h"

#include <array>
#include <stdexcept>

using namespace steeplejack;

DescriptorSetLayout::DescriptorSetLayout(const Device& device, std::string_view layout_name) :
    m_device(device),
    m_layout_definition(config::DescriptorLayoutConfig::instance().require_layout(layout_name)),
    m_descriptor_set_layout(create_descriptor_set_layout()),
    m_descriptor_set_layouts({m_descriptor_set_layout}),
    m_write_descriptor_sets(create_write_descriptor_sets())
{
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(m_device.vk(), m_descriptor_set_layout, nullptr);
}

VkPipelineLayout DescriptorSetLayout::create_pipeline_layout() const
{
    spdlog::info("Creating Graphics Pipeline Layout");

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(m_descriptor_set_layouts.size());
    pipeline_layout_info.pSetLayouts = m_descriptor_set_layouts.data();
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    VkPipelineLayout pipeline_layout = nullptr;
    if (vkCreatePipelineLayout(m_device.vk(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    return pipeline_layout;
}

VkDescriptorSetLayout DescriptorSetLayout::create_descriptor_set_layout()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto& binding_info : m_layout_definition.bindings)
    {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = binding_info.binding;
        binding.descriptorType = binding_info.type;
        binding.descriptorCount = binding_info.count;
        binding.stageFlags = binding_info.stage_flags;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();

    VkDescriptorSetLayout layout = nullptr;
    if (vkCreateDescriptorSetLayout(m_device.vk(), &layout_info, nullptr, &layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout");
    }

    return layout;
}

std::vector<VkWriteDescriptorSet> DescriptorSetLayout::create_write_descriptor_sets()
{
    std::vector<VkWriteDescriptorSet> write_descriptor_sets;
    for (const auto& binding_info : m_layout_definition.bindings)
    {
        VkWriteDescriptorSet write_descriptor_set = {};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet = nullptr;
        write_descriptor_set.dstBinding = binding_info.binding;
        write_descriptor_set.dstArrayElement = 0;
        write_descriptor_set.descriptorType = binding_info.type;
        write_descriptor_set.descriptorCount = binding_info.count;
        write_descriptor_set.pBufferInfo = nullptr;
        write_descriptor_set.pImageInfo = nullptr;
        write_descriptor_set.pTexelBufferView = nullptr;
        write_descriptor_sets.push_back(write_descriptor_set);
    }

    return write_descriptor_sets;
}

void DescriptorSetLayout::reset_writes()
{
    for (auto& write : m_write_descriptor_sets)
    {
        write.pImageInfo = nullptr;
        write.pBufferInfo = nullptr;
        write.pTexelBufferView = nullptr;
    }
}

std::vector<VkWriteDescriptorSet> DescriptorSetLayout::get_write_descriptor_sets() const
{
    std::vector<VkWriteDescriptorSet> result;
    result.reserve(m_write_descriptor_sets.size());

    for (const auto& write : m_write_descriptor_sets)
    {
        switch (write.descriptorType)
        {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            if (write.pBufferInfo != nullptr)
            {
                result.push_back(write);
            }
            break;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case VK_DESCRIPTOR_TYPE_SAMPLER:
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            if (write.pImageInfo != nullptr)
            {
                result.push_back(write);
            }
            break;
        default:
            // Unknown/unsupported types not expected here; ignore if unset
            break;
        }
    }

    return result;
}

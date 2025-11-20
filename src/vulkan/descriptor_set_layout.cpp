#include "descriptor_set_layout.h"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <string>

using namespace steeplejack;
using config::DescriptorBindingDefinition;
using config::DescriptorLayoutConfig;

DescriptorSetLayout::DescriptorSetLayout(const Device& device, std::string_view layout_name) :
    m_device(device),
    m_layout_definition(DescriptorLayoutConfig::instance().require_layout(layout_name)),
    m_descriptor_set_layout(create_descriptor_set_layout()),
    m_descriptor_set_layouts({m_descriptor_set_layout}),
    m_write_descriptor_sets(create_write_descriptor_sets()),
    m_active_write_sets(m_write_descriptor_sets.size())
{
    spdlog::info("Creating Descriptor Set Layout '{}'", layout_name);

    uint32_t max_binding = 0;
    for (const auto& binding : m_layout_definition.bindings)
    {
        max_binding = std::max(max_binding, binding.binding);
    }
    m_binding_to_write_index.assign(static_cast<size_t>(max_binding) + 1, std::numeric_limits<size_t>::max());

    for (size_t i = 0; i < m_layout_definition.bindings.size(); ++i)
    {
        const auto& binding = m_layout_definition.bindings[i];
        m_binding_to_write_index[binding.binding] = i;
        BindingHandle handle{};
        handle.binding = binding.binding;
        handle.write_index = i;
        handle.type = binding.type;
        m_binding_handles.emplace(binding.name, handle);
    }
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    spdlog::info("Destroying Descriptor Set Layout '{}'", m_layout_definition.name);
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

const DescriptorSetLayout::BindingHandle& DescriptorSetLayout::binding_handle(std::string_view name) const
{
    auto it = m_binding_handles.find(std::string(name));
    if (it == m_binding_handles.end())
    {
        throw std::runtime_error(
            "Descriptor binding '" + std::string(name) + "' not found in layout '" + m_layout_definition.name + "'");
    }
    return it->second;
}

DescriptorSetLayout& DescriptorSetLayout::write_combined_image_sampler(
    VkDescriptorImageInfo* image_info, const BindingHandle& binding_handle)
{
    auto& write_descriptor_set = m_write_descriptor_sets[binding_handle.write_index];
    write_descriptor_set.dstBinding = binding_handle.binding;
    write_descriptor_set.pImageInfo = image_info;

    return *this;
}

DescriptorSetLayout&
DescriptorSetLayout::write_uniform_buffer(VkDescriptorBufferInfo* buffer_info, const BindingHandle& binding_handle)
{
    auto& write_descriptor_set = m_write_descriptor_sets[binding_handle.write_index];
    write_descriptor_set.dstBinding = binding_handle.binding;
    write_descriptor_set.pBufferInfo = buffer_info;

    return *this;
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

std::span<const VkWriteDescriptorSet> DescriptorSetLayout::get_write_descriptor_sets() const
{
    size_t size = 0;
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
                m_active_write_sets[size++] = write;
            }
            break;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case VK_DESCRIPTOR_TYPE_SAMPLER:
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            if (write.pImageInfo != nullptr)
            {
                m_active_write_sets[size++] = write;
            }
            break;
        default:
            // Unknown/unsupported types not expected here; ignore if unset
            break;
        }
    }

    return {m_active_write_sets.data(), size};
}

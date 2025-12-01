#include "descriptor_set_layout.h"

#include "device.h"
#include "spdlog/spdlog.h"

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <string>

using namespace steeplejack;
using config::DescriptorBinding;

namespace
{
VkDescriptorType parse_descriptor_type(const std::string& type)
{
    if (type == "uniform_buffer")
    {
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
    if (type == "combined_image_sampler")
    {
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
    throw std::runtime_error("Unsupported descriptor type: " + type);
}

VkShaderStageFlags parse_stage_flags(const std::vector<std::string>& flags)
{
    VkShaderStageFlags result = 0;
    for (const auto& f : flags)
    {
        if (f == "vertex")
        {
            result |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        else if (f == "fragment")
        {
            result |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
    }
    return result;
}

VkDescriptorSetLayout
create_descriptor_set_layout(const Device& device, std::span<const config::DescriptorBinding> bindings_def)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(bindings_def.size());
    for (const auto& binding_info : bindings_def)
    {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = binding_info.binding;
        binding.descriptorType = parse_descriptor_type(binding_info.type);
        binding.descriptorCount = 1;
        binding.stageFlags = parse_stage_flags(binding_info.stage_flags);
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();

    VkDescriptorSetLayout layout = nullptr;
    if (vkCreateDescriptorSetLayout(device.vk(), &layout_info, nullptr, &layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout");
    }

    return layout;
}

std::vector<VkWriteDescriptorSet>
create_write_descriptor_sets(std::span<const config::DescriptorBinding> bindings_def)
{
    std::vector<VkWriteDescriptorSet> write_descriptor_sets;
    write_descriptor_sets.reserve(bindings_def.size());

    for (const auto& binding_info : bindings_def)
    {
        VkWriteDescriptorSet write_descriptor_set = {};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet = nullptr;
        write_descriptor_set.dstBinding = binding_info.binding;
        write_descriptor_set.dstArrayElement = 0;
        write_descriptor_set.descriptorType = parse_descriptor_type(binding_info.type);
        write_descriptor_set.descriptorCount = 1;
        write_descriptor_set.pBufferInfo = nullptr;
        write_descriptor_set.pImageInfo = nullptr;
        write_descriptor_set.pTexelBufferView = nullptr;
        write_descriptor_sets.push_back(write_descriptor_set);
    }

    return write_descriptor_sets;
}
} // namespace

DescriptorSetLayout::DescriptorSetLayout(const Device& device, std::string_view /*layout_name*/) :
    m_device(device),
    m_descriptor_set_layout(nullptr),
    m_descriptor_set_layouts({m_descriptor_set_layout})
{
    throw std::runtime_error("Legacy descriptor set layouts are no longer supported");
}

DescriptorSetLayout::DescriptorSetLayout(const Device& device, std::span<const DescriptorBinding> bindings) :
    m_device(device),
    m_descriptor_set_layout(create_descriptor_set_layout(device, bindings)),
    m_descriptor_set_layouts({m_descriptor_set_layout}),
    m_write_descriptor_sets(create_write_descriptor_sets(bindings)),
    m_active_write_sets(m_write_descriptor_sets.size())
{
    spdlog::info("Creating Descriptor Set Layout (span)");

    uint32_t max_binding = 0;
    for (const auto& binding : bindings)
    {
        max_binding = std::max(max_binding, binding.binding);
    }
    m_binding_to_write_index.assign(static_cast<size_t>(max_binding) + 1, std::numeric_limits<size_t>::max());

    for (size_t i = 0; i < bindings.size(); ++i)
    {
        const auto& binding = bindings[i];
        m_binding_to_write_index[binding.binding] = i;
        BindingHandle handle{};
        handle.binding = binding.binding;
        handle.write_index = i;
        handle.type = parse_descriptor_type(binding.type);
        m_binding_handles.emplace(binding.name, handle);
    }
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    spdlog::info("Destroying Descriptor Set Layout");
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

const DescriptorSetLayout::BindingHandle& DescriptorSetLayout::binding_handle(std::string name) const
{
    auto it = m_binding_handles.find(name);
    if (it == m_binding_handles.end())
    {
        throw std::runtime_error("Descriptor binding '" + name + "' not found");
    }
    return it->second;
}

void DescriptorSetLayout::write_combined_image_sampler(
    VkDescriptorImageInfo* image_info, const BindingHandle& binding_handle)
{
    auto& write_descriptor_set = m_write_descriptor_sets[binding_handle.write_index];
    write_descriptor_set.dstBinding = binding_handle.binding;
    write_descriptor_set.pImageInfo = image_info;
}

void DescriptorSetLayout::write_uniform_buffer(VkDescriptorBufferInfo* buffer_info, const BindingHandle& binding_handle)
{
    auto& write_descriptor_set = m_write_descriptor_sets[binding_handle.write_index];
    write_descriptor_set.dstBinding = binding_handle.binding;
    write_descriptor_set.pBufferInfo = buffer_info;
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

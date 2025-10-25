#include "descriptor_set_layout.h"

#include <array>
#include <stdexcept>

#include "spdlog/spdlog.h"

using namespace levin;

DescriptorSetLayout::DescriptorSetLayout(
    const Device &device,
    std::vector<DescriptorSetLayoutInfo> layout_infos):
    m_device(device),
    m_layout_infos(layout_infos),
    m_descriptor_set_layout(create_descriptor_set_layout()),
    m_descriptor_set_layouts({ m_descriptor_set_layout }),
    m_write_descriptor_sets(create_write_descriptor_sets())
{
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layout, nullptr);
}

VkDescriptorSetLayout DescriptorSetLayout::create_descriptor_set_layout()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto &info : m_layout_infos)
    {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = info.binding;
        binding.descriptorType = info.descriptor_type;
        binding.descriptorCount = 1;
        binding.stageFlags = info.stage_flags;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();

    VkDescriptorSetLayout layout;
    if (vkCreateDescriptorSetLayout(m_device, &layout_info, nullptr, &layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout");
    }

    return layout;
}

std::vector<VkWriteDescriptorSet> DescriptorSetLayout::create_write_descriptor_sets()
{
    std::vector<VkWriteDescriptorSet> write_descriptor_sets;
    for (const auto &info : m_layout_infos)
    {
        VkWriteDescriptorSet write_descriptor_set = {};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet = VK_NULL_HANDLE;
        write_descriptor_set.dstBinding = info.binding;
        write_descriptor_set.dstArrayElement = 0;
        write_descriptor_set.descriptorType = info.descriptor_type;
        write_descriptor_set.descriptorCount = 1;
        write_descriptor_set.pBufferInfo = nullptr;
        write_descriptor_set.pImageInfo = nullptr;
        write_descriptor_set.pTexelBufferView = nullptr;
        write_descriptor_sets.push_back(write_descriptor_set);
    }

    return write_descriptor_sets;
}
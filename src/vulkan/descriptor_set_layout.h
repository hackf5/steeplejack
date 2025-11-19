#pragma once

#include "config/descriptor_layout_config.h"
#include "vulkan/device.h"

#include <array>
#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class DescriptorSetLayout
{
  private:
    const Device& m_device;
    const config::DescriptorLayoutDefinition& m_layout_definition;
    const VkDescriptorSetLayout m_descriptor_set_layout;
    const std::array<VkDescriptorSetLayout, 1> m_descriptor_set_layouts;
    std::vector<VkWriteDescriptorSet> m_write_descriptor_sets;

    VkDescriptorSetLayout create_descriptor_set_layout();
    std::vector<VkWriteDescriptorSet> create_write_descriptor_sets();

  public:
    DescriptorSetLayout(const Device& device, std::string_view layout_name);
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

    explicit operator VkDescriptorSetLayout() const
    {
        return m_descriptor_set_layout;
    }

    [[nodiscard]] VkPipelineLayout create_pipeline_layout() const;

    void reset_writes();

    [[nodiscard]] std::vector<VkWriteDescriptorSet> get_write_descriptor_sets() const;

    DescriptorSetLayout& write_combined_image_sampler(VkDescriptorImageInfo* image_info, uint32_t binding_index)
    {
        auto& write_descriptor_set = m_write_descriptor_sets[binding_index];
        write_descriptor_set.dstBinding = binding_index;
        write_descriptor_set.pImageInfo = image_info;

        return *this;
    }

    DescriptorSetLayout& write_uniform_buffer(VkDescriptorBufferInfo* buffer_info, uint32_t binding_index)
    {
        auto& write_descriptor_set = m_write_descriptor_sets[binding_index];
        write_descriptor_set.dstBinding = binding_index;
        write_descriptor_set.pBufferInfo = buffer_info;

        return *this;
    }
};
} // namespace steeplejack

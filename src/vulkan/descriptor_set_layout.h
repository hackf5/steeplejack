#pragma once

#include "descriptor_set_layout_info.h"
#include "util/no_copy_or_move.h"
#include "vulkan/device.h"

#include <array>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class DescriptorSetLayout : NoCopyOrMove
{
  private:
    const Device& m_device;
    const std::vector<DescriptorSetLayoutInfo> m_layout_infos;
    const VkDescriptorSetLayout m_descriptor_set_layout;
    const std::array<VkDescriptorSetLayout, 1> m_descriptor_set_layouts;
    std::vector<VkWriteDescriptorSet> m_write_descriptor_sets;

    VkDescriptorSetLayout create_descriptor_set_layout();
    std::vector<VkWriteDescriptorSet> create_write_descriptor_sets();

  public:
    DescriptorSetLayout(const Device& device, std::vector<DescriptorSetLayoutInfo> layout_infos);
    ~DescriptorSetLayout();

    explicit operator VkDescriptorSetLayout() const
    {
        return m_descriptor_set_layout;
    }

    [[nodiscard]] VkPipelineLayout create_pipeline_layout() const;

    void reset_writes();

    std::vector<VkWriteDescriptorSet> get_write_descriptor_sets() const;

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

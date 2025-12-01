#pragma once

#include "config/pipeline_config.h"

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Device;

class DescriptorSetLayout
{
  public:
    struct BindingHandle
    {
        uint32_t binding = 0;
        size_t write_index = 0;
        VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    };

  private:
    const Device& m_device;
    const VkDescriptorSetLayout m_descriptor_set_layout;
    const std::array<VkDescriptorSetLayout, 1> m_descriptor_set_layouts;

    std::vector<VkWriteDescriptorSet> m_write_descriptor_sets;
    std::vector<size_t> m_binding_to_write_index;
    std::unordered_map<std::string, BindingHandle> m_binding_handles;

    mutable std::vector<VkWriteDescriptorSet> m_active_write_sets;

  public:
    DescriptorSetLayout(const Device& device, std::string_view layout_name); // legacy path, currently unsupported
    DescriptorSetLayout(const Device& device, std::span<const config::DescriptorBinding> bindings);
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

    [[nodiscard]] VkPipelineLayout create_pipeline_layout() const;
    [[nodiscard]] const BindingHandle& binding_handle(std::string name) const;

    [[nodiscard]] std::span<const VkWriteDescriptorSet> get_write_descriptor_sets() const;

    void write_combined_image_sampler(VkDescriptorImageInfo* image_info, const BindingHandle& binding_handle);
    void write_uniform_buffer(VkDescriptorBufferInfo* buffer_info, const BindingHandle& binding_handle);
    void reset_writes();
};
} // namespace steeplejack

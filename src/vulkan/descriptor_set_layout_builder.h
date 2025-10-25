#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "device.h"
#include "descriptor_set_layout.h"
#include "descriptor_set_layout_info.h"

namespace steeplejack
{
class DescriptorSetLayoutBuilder
{
private:
    std::vector<DescriptorSetLayoutInfo> m_infos;

    void add_info(VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags)
    {
        auto binding = static_cast<uint32_t>(m_infos.size());
        m_infos.push_back({ descriptor_type, stage_flags, binding });
    }

public:
    DescriptorSetLayoutBuilder &add_combined_image_sampler()
    {
        add_info(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
        return *this;
    }

    DescriptorSetLayoutBuilder &add_uniform_buffer()
    {
        add_info(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> build(const Device &device)
    {
        auto result = std::make_unique<DescriptorSetLayout>(device, m_infos);
        m_infos.clear();
        return result;
    }
};
}
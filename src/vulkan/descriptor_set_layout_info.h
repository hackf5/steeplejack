#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
struct DescriptorSetLayoutInfo
{
    const VkDescriptorType descriptor_type;
    const VkShaderStageFlags stage_flags;
    const uint32_t binding;
};
} // namespace steeplejack
#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

namespace steeplejack
{
struct DescriptorSetLayoutInfo
{
    VkDescriptorType descriptor_type;
    VkShaderStageFlags stage_flags;
    uint32_t binding;
};
} // namespace steeplejack
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack::config
{
struct DescriptorBindingDefinition
{
    std::string name;
    uint32_t binding = 0;
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    uint32_t count = 1;
    VkShaderStageFlags stage_flags = 0;
};

struct DescriptorLayoutDefinition
{
    std::string name;
    std::vector<DescriptorBindingDefinition> bindings;

    [[nodiscard]] const DescriptorBindingDefinition& require_binding(std::string_view binding_name) const;
};

class DescriptorLayoutConfig
{
  public:
    static const DescriptorLayoutConfig& instance();

    [[nodiscard]] const DescriptorLayoutDefinition& require_layout(std::string_view name) const;
    [[nodiscard]] const std::unordered_map<std::string, DescriptorLayoutDefinition>& layouts() const noexcept
    {
        return m_layouts;
    }

  private:
    DescriptorLayoutConfig();

    std::unordered_map<std::string, DescriptorLayoutDefinition> m_layouts;
};

} // namespace steeplejack::config

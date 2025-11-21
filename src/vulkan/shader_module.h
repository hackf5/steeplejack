#pragma once

#include "device.h"

#include <string_view>

namespace steeplejack
{
class ShaderModule
{
  private:
    const Device& m_device;
    const std::string m_name;
    VkShaderModule m_shader_module;

  public:
    ShaderModule(const Device& device, std::string_view name);
    ~ShaderModule();

    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;
    ShaderModule(ShaderModule&&) = delete;
    ShaderModule& operator=(ShaderModule&&) = delete;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] VkShaderModule vk() const;
};
} // namespace steeplejack

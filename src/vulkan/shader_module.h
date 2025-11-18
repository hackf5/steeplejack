#pragma once

#include "device.h"

#include <string>

namespace steeplejack
{
class ShaderModule
{
  private:
    const Device& m_device;

    const std::string m_name;

    VkShaderModule m_shader_module;

    static std::vector<char> read_file(const std::string& name);

    [[nodiscard]] VkShaderModule create_shader_module() const;

  public:
    ShaderModule(const Device& device, std::string name);
    ~ShaderModule();

    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;
    ShaderModule(ShaderModule&&) = delete;
    ShaderModule& operator=(ShaderModule&&) = delete;

    [[nodiscard]] const std::string& name() const
    {
        return m_name;
    }

    [[nodiscard]] VkShaderModule vk() const
    {
        return m_shader_module;
    }
};
} // namespace steeplejack

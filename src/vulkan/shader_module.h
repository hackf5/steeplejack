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

    VkShaderModule create_shader_module();

  public:
    ShaderModule(const Device& device, std::string name);
    ~ShaderModule();

    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;
    ShaderModule(ShaderModule&&) = delete;
    ShaderModule& operator=(ShaderModule&&) = delete;

    const std::string& name() const
    {
        return m_name;
    }

    operator VkShaderModule() const
    {
        return m_shader_module;
    }
};
} // namespace steeplejack

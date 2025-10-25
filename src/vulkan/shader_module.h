#pragma once

#include <string>

#include "util/no_copy_or_move.h"
#include "device.h"

namespace steeplejack
{
class ShaderModule: NoCopyOrMove
{
private:
    const Device &m_device;

    const std::string m_name;

    VkShaderModule m_shader_module;

    static std::vector<char> read_file(const std::string &name);

    VkShaderModule create_shader_module();

public:
    ShaderModule(
        const Device &device,
        const std::string &name);
    ~ShaderModule();

    const std::string &name() const { return m_name; }

    operator VkShaderModule() const { return m_shader_module; }
};
}
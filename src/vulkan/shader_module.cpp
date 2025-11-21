#include "shader_module.h"

#include "spdlog/spdlog.h"

#include <format>
#include <fstream>
#include <string_view>

using namespace steeplejack;

namespace
{
[[nodiscard]] std::vector<char> read_file(std::string_view name)
{
    auto file_name = std::format("shaders/{}.spv", name);
    std::ifstream file(file_name, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        spdlog::error("Failed to open file: {} ({})", name, file_name);
        throw std::runtime_error("Failed to open file");
    }

    size_t const file_size = (size_t)file.tellg();
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(file_size));

    return buffer;
}
[[nodiscard]] VkShaderModule create_shader_module(const Device& device, std::string_view name)
{
    spdlog::info("Creating Shader Module: {}", name);

    auto code = read_file(name);

    VkShaderModuleCreateInfo shader_module_info{};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_info.codeSize = code.size();
    shader_module_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shader_module = nullptr;
    if (vkCreateShaderModule(device.vk(), &shader_module_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module for " + std::string(name) + " shader");
    }

    return shader_module;
}
} // namespace

ShaderModule::ShaderModule(const Device& device, std::string_view name) :
    m_device(device), m_name(std::string(name)), m_shader_module(create_shader_module(device, name))
{
}

ShaderModule::~ShaderModule()
{
    spdlog::info("Destroying Shader Module: {}", m_name);
    vkDestroyShaderModule(m_device.vk(), m_shader_module, nullptr);
}

const std::string& ShaderModule::name() const
{
    return m_name;
}

VkShaderModule ShaderModule::vk() const
{
    return m_shader_module;
}

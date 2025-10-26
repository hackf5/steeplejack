#include "shader_module.h"

#include "spdlog/spdlog.h"

#include <fstream>
#include <utility>

using namespace steeplejack;

ShaderModule::ShaderModule(const Device& device, std::string name) :
    m_device(device), m_name(std::move(name)), m_shader_module(create_shader_module())
{
}

ShaderModule::~ShaderModule()
{
    spdlog::info("Destroying Shader Module: {}", m_name);
    vkDestroyShaderModule(m_device, m_shader_module, nullptr);
}

std::vector<char> ShaderModule::read_file(const std::string& name)
{
    auto file_name = "shaders/" + name + ".spv";
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

VkShaderModule ShaderModule::create_shader_module()
{
    spdlog::info("Creating Shader Module: {}", m_name);

    auto code = read_file(m_name);

    VkShaderModuleCreateInfo shader_module_info{};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_info.codeSize = code.size();
    shader_module_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shader_module = nullptr;
    if (vkCreateShaderModule(m_device, &shader_module_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module for " + m_name + " shader");
    }

    return shader_module;
}
#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace levin
{
enum class VertexComponent { Position, UV, Color };

struct Vertex
{
    typedef uint32_t index_t;

    static const VkIndexType vk_index_type = VK_INDEX_TYPE_UINT32;

    static const std::vector<VertexComponent> ALL_COMPONENTS;

    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec4 color;
};


struct VertexInputState
{
    VkVertexInputBindingDescription binding;
    std::vector<VkVertexInputAttributeDescription> attributes;
    VkPipelineVertexInputStateCreateInfo pipeline;

    VertexInputState(
        uint32_t binding,
        const std::vector<VertexComponent> components);

private:
    VkVertexInputBindingDescription create_binding(uint32_t binding);

    VkVertexInputAttributeDescription create_attribute(
        uint32_t location,
        VertexComponent component);

    std::vector<VkVertexInputAttributeDescription> create_attributes(
        const std::vector<VertexComponent> components);

    VkPipelineVertexInputStateCreateInfo create_pipeline();
};
}
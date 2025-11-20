#pragma once

#include "glm_config.hpp"

#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
enum class VertexComponent : std::uint8_t
{
    Position,
    Normal,
    UV,
    Color
};

struct Vertex
{
    using Index = uint32_t;

    static constexpr VkIndexType kVkIndexType = VK_INDEX_TYPE_UINT32;

    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 color;
};

struct VertexInputState
{
    VkVertexInputBindingDescription binding;
    std::vector<VkVertexInputAttributeDescription> attributes;
    VkPipelineVertexInputStateCreateInfo pipeline;

    VertexInputState(uint32_t binding, std::span<const VertexComponent> components);

    static VertexInputState make_all(uint32_t binding);
    static VertexInputState make_position_only(uint32_t binding);
};
} // namespace steeplejack

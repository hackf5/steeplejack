#pragma once

#include "glm_config.hpp"

#include <array>
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
    using IndexT = uint32_t;

    static constexpr VkIndexType kVkIndexType = VK_INDEX_TYPE_UINT32;

    static constexpr std::array<VertexComponent, 4> kAllComponents{
        VertexComponent::Position, VertexComponent::UV, VertexComponent::Color, VertexComponent::Normal};

    static constexpr std::array<VertexComponent, 1> kPositionOnly{VertexComponent::Position};

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

  private:
    static VkVertexInputBindingDescription create_binding(uint32_t binding);

    VkVertexInputAttributeDescription create_attribute(uint32_t location, VertexComponent component) const;

    std::vector<VkVertexInputAttributeDescription> create_attributes(std::span<const VertexComponent> components);

    VkPipelineVertexInputStateCreateInfo create_pipeline();
};
} // namespace steeplejack

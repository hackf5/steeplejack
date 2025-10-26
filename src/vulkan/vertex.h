#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace steeplejack
{
enum class VertexComponent
{
    Position,
    UV,
    Color
};

struct Vertex
{
    typedef uint32_t index_t;

    static const VkIndexType vk_index_type = VK_INDEX_TYPE_UINT32;

    static constexpr std::array<VertexComponent, 3> kAllComponents{
        VertexComponent::Position, VertexComponent::UV, VertexComponent::Color};

    glm::vec3 pos;
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

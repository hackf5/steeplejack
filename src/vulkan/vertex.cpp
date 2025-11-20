#include "vertex.h"

#include <stdexcept>

using namespace steeplejack;

namespace
{
VkVertexInputBindingDescription make_binding(uint32_t binding_index)
{
    return VkVertexInputBindingDescription({
        .binding = binding_index,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    });
}

VkVertexInputAttributeDescription make_attribute(uint32_t location, VertexComponent component, uint32_t binding_index)
{
    VkVertexInputAttributeDescription description{};
    description.location = location;
    description.binding = binding_index;

    switch (component)
    {
    case VertexComponent::Position:
        description.format = VK_FORMAT_R32G32B32_SFLOAT;
        description.offset = offsetof(Vertex, pos);
        break;
    case VertexComponent::Normal:
        description.format = VK_FORMAT_R32G32B32_SFLOAT;
        description.offset = offsetof(Vertex, normal);
        break;
    case VertexComponent::UV:
        description.format = VK_FORMAT_R32G32_SFLOAT;
        description.offset = offsetof(Vertex, uv);
        break;
    case VertexComponent::Color:
        description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        description.offset = offsetof(Vertex, color);
        break;
    default:
        throw std::runtime_error("Unknown vertex component " + std::to_string(static_cast<int>(component)));
    }

    return description;
}

std::vector<VkVertexInputAttributeDescription>
make_attributes(std::span<const VertexComponent> components, uint32_t binding_index)
{
    std::vector<VkVertexInputAttributeDescription> descriptions;
    descriptions.reserve(components.size());
    for (uint32_t location = 0; location < components.size(); location++)
    {
        descriptions.push_back(make_attribute(location, components[location], binding_index));
    }

    return descriptions;
}

VkPipelineVertexInputStateCreateInfo make_pipeline(
    const VkVertexInputBindingDescription& binding, const std::vector<VkVertexInputAttributeDescription>& attributes)
{
    return VkPipelineVertexInputStateCreateInfo({
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
        .pVertexAttributeDescriptions = attributes.data(),
    });
}
} // namespace

VertexInputState::VertexInputState(uint32_t binding_index, std::span<const VertexComponent> components) :
    binding(make_binding(binding_index)),
    attributes(make_attributes(components, binding_index)),
    pipeline(make_pipeline(binding, attributes))
{
}

VertexInputState VertexInputState::make_all(uint32_t binding)
{
    static constexpr std::array<VertexComponent, 4> kComponents{
        VertexComponent::Position,
        VertexComponent::UV,
        VertexComponent::Color,
        VertexComponent::Normal};
    return {binding, kComponents};
}

VertexInputState VertexInputState::make_position_only(uint32_t binding)
{
    static constexpr std::array<VertexComponent, 1> kComponents{VertexComponent::Position};
    return {binding, kComponents};
}

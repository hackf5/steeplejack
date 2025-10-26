#include "vertex.h"

#include <stdexcept>

using namespace steeplejack;

VertexInputState::VertexInputState(uint32_t binding, std::span<const VertexComponent> components) :
    binding(create_binding(binding)), attributes(create_attributes(components)), pipeline(create_pipeline())
{
}

VkVertexInputBindingDescription VertexInputState::create_binding(uint32_t binding)
{
    return VkVertexInputBindingDescription({
        .binding = binding,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    });
}

VkVertexInputAttributeDescription VertexInputState::create_attribute(uint32_t location, VertexComponent component) const
{
    VkVertexInputAttributeDescription description{};
    description.location = location;
    description.binding = binding.binding;

    switch (component)
    {
    case VertexComponent::Position:
        description.format = VK_FORMAT_R32G32B32_SFLOAT;
        description.offset = offsetof(Vertex, pos);
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
VertexInputState::create_attributes(std::span<const VertexComponent> components)
{
    std::vector<VkVertexInputAttributeDescription> descriptions;
    descriptions.reserve(components.size());
    for (uint32_t location = 0; location < components.size(); location++)
    {
        descriptions.push_back(create_attribute(location, components[location]));
    }

    return descriptions;
}

VkPipelineVertexInputStateCreateInfo VertexInputState::create_pipeline()
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

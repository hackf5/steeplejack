// Forward render system (stub)
#pragma once

#include <entt/entt.hpp>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class GraphicsPipeline; // forward decl
}

namespace steeplejack::ecs
{
// Placeholder signature; wiring to your GraphicsPipeline will come next
inline void render_forward(
    entt::registry& /*reg*/, VkCommandBuffer /*cmd*/, uint32_t /*frame_index*/, GraphicsPipeline& /*pipeline*/)
{
    // To be implemented during wiring
}
} // namespace steeplejack::ecs


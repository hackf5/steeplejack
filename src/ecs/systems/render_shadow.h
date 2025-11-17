// Shadow render system (stub)
#pragma once

#include <entt/entt.hpp>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowPipeline; // forward decl
}

namespace steeplejack::ecs
{
// Placeholder signature; wiring to your ShadowPipeline will come next
inline void render_shadow(
    entt::registry& /*reg*/,
    VkCommandBuffer /*cmd*/,
    uint32_t /*frame_index*/,
    ShadowPipeline& /*pipeline*/,
    size_t /*spot_index*/)
{
    // To be implemented during wiring
}
} // namespace steeplejack::ecs

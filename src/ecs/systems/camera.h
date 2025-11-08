// Camera system (scaffold)
#pragma once

#include <entt/entt.hpp>

#include "ecs/components.h"

namespace steeplejack::ecs
{
inline void update_cameras(entt::registry& reg)
{
    auto view = reg.view<Camera>();
    view.each([](Camera& c) {
        if (!c.dirty)
            return;
        c.proj = glm::perspective(glm::radians(c.fov), c.aspect, c.clip_near, c.clip_far);
        c.proj[1][1] *= -1.0f; // Vulkan clip space
        c.view = glm::lookAt(c.position, c.target, glm::vec3(0.0f, 0.0f, 1.0f));
        c.dirty = false;
    });
}
} // namespace steeplejack::ecs

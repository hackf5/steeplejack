// Lights system (scaffold)
#pragma once

#include "ecs/components.h"

#include <entt/entt.hpp>

namespace steeplejack::ecs
{
inline void update_spot_lights(entt::registry& reg)
{
    auto view = reg.view<SpotLight>();
    view.each(
        [](SpotLight& s)
        {
            if (!s.enable)
            {
                return;
            }
            const glm::vec3 center = s.position + glm::normalize(s.direction);
            glm::vec3 up{0.0F, 0.0F, 1.0F};
            if (glm::abs(glm::dot(up, glm::normalize(s.direction))) > 0.99F)
            {
                up = glm::vec3(0.0F, 1.0F, 0.0F);
            }
            const glm::mat4 viewm = glm::lookAt(s.position, center, up);
            const float fovy = glm::acos(s.outerCos) * 2.0F;
            const float z_near = glm::max(0.05F, 0.02F * s.range);
            glm::mat4 projm = glm::perspective(fovy, 1.0F, z_near, s.range);
            projm[1][1] *= -1.0F; // Vulkan clip space
            s.viewProj = projm * viewm;
        });
}
} // namespace steeplejack::ecs

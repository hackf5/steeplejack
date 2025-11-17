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
                return;
            const glm::vec3 center = s.position + glm::normalize(s.direction);
            glm::vec3 up{0.0f, 0.0f, 1.0f};
            if (glm::abs(glm::dot(up, glm::normalize(s.direction))) > 0.99f)
            {
                up = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            const glm::mat4 viewm = glm::lookAt(s.position, center, up);
            const float fovY = glm::acos(s.outerCos) * 2.0f;
            const float nearp = glm::max(0.05f, 0.02f * s.range);
            glm::mat4 projm = glm::perspective(fovY, 1.0f, nearp, s.range);
            projm[1][1] *= -1.0f; // Vulkan clip space
            s.viewProj = projm * viewm;
        });
}
} // namespace steeplejack::ecs

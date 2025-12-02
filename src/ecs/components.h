#pragma once

#include "ecs/types.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

namespace steeplejack::ecs
{
struct Name
{
    std::string value;
};

struct Transform
{
    glm::vec3 position{0.0F, 0.0F, 0.0F};
    glm::quat rotation{1.0F, 0.0F, 0.0F, 0.0F};
    glm::vec3 scale{1.0F, 1.0F, 1.0F};

    glm::mat4 local_matrix{1.0F};
    glm::mat4 world_matrix{1.0F};

    // Optional parent relationship; ignored when invalid.
    EntityId parent{kInvalidEntity};
};
} // namespace steeplejack::ecs

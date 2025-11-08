// ECS scene holder (scaffold)
#pragma once

#include <entt/entt.hpp>

#include "ecs/resources.h"

namespace steeplejack::ecs
{
struct Scene
{
    entt::registry registry{};
    Resources resources{};
};
} // namespace steeplejack::ecs


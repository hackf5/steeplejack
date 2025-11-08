// ECS scene holder (scaffold)
#pragma once

#include <entt/entt.hpp>

#include "ecs/resources.h"
#include "ecs/geometry_store.h"
#include "ecs/material_store.h"
#include "ecs/gpu/ubo_caches.h"

namespace steeplejack::ecs
{
struct Scene
{
    entt::registry registry{};
    Resources resources{};
    GeometryStore geometry{};
    MaterialStore materials{};
    ModelUboCache model_ubos{};
    CameraUboCache camera_ubos{};
};
} // namespace steeplejack::ecs

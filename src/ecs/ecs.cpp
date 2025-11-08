// Minimal TU to ensure ECS headers compile with the build

#include <entt/entt.hpp>

#include "ecs/components.h"
#include "ecs/resources.h"
#include "ecs/scene.h"
#include "ecs/systems/transform.h"
#include "ecs/systems/camera.h"
#include "ecs/systems/lights.h"
#include "ecs/systems/build_draw_lists.h"
#include "ecs/systems/render_forward.h"
#include "ecs/systems/render_shadow.h"

namespace steeplejack::ecs
{
void ecs_sanity()
{
    entt::registry reg;
    (void)reg;
}
} // namespace steeplejack::ecs


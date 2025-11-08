// Minimal TU to ensure ECS headers compile with the build

#include "ecs/components.h"
#include "ecs/gpu/ubo_caches.h"
#include "ecs/resources.h"
#include "ecs/scene.h"
#include "ecs/systems/build_draw_lists.h"
#include "ecs/systems/camera.h"
#include "ecs/systems/flush_ubos.h"
#include "ecs/systems/lights.h"
#include "ecs/systems/render_forward.h"
#include "ecs/systems/render_shadow.h"
#include "ecs/systems/transform.h"

#include <entt/entt.hpp>

namespace steeplejack::ecs
{
void ecs_sanity()
{
    entt::registry reg;

    // Exercise systems so headers are compiled and validated by the TU
    update_transforms(reg);
    update_cameras(reg);
    update_spot_lights(reg);
    (void)build_draw_list(reg);
}
} // namespace steeplejack::ecs

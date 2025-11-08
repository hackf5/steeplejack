// Systems to flush ECS data into GPU UBOs
#pragma once

#include <entt/entt.hpp>

#include "ecs/components.h"
#include "ecs/gpu/ubo_caches.h"
#include "vulkan/device.h"

namespace steeplejack::ecs
{
inline void flush_model_ubos(entt::registry& reg, const Device& device, ModelUboCache& cache, uint32_t frame_index)
{
    auto view = reg.view<Transform, RenderMesh>();
    for (auto e : view)
    {
        auto& t = view.get<Transform>(e);
        auto* u = cache.ensure(e, device);
        ModelUbo::Block blk{.model = t.world};
        (*u->buffer)[frame_index].copy_from(blk);
    }
}

inline entt::entity find_main_camera(entt::registry& reg)
{
    entt::entity cam = entt::null;
    auto view = reg.view<Camera, MainCamera>();
    for (auto e : view)
    {
        cam = e;
        break;
    }
    if (cam == entt::null)
    {
        auto all = reg.view<Camera>();
        for (auto e : all)
        {
            cam = e;
            break;
        }
    }
    return cam;
}

inline CameraUbo* flush_camera_ubo(entt::registry& reg, const Device& device, CameraUboCache& cache, uint32_t frame_index)
{
    entt::entity cam_e = find_main_camera(reg);
    if (cam_e == entt::null)
        return nullptr;

    auto& c = reg.get<Camera>(cam_e);
    auto* cu = cache.ensure(cam_e, device);
    CameraUbo::Block blk{.proj = c.proj, .view = c.view};
    (*cu->buffer)[frame_index].copy_from(blk);
    return cu;
}
} // namespace steeplejack::ecs


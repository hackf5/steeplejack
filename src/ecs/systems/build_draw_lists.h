// Draw list builder (scaffold)
#pragma once

#include <entt/entt.hpp>

#include "ecs/components.h"

#include <vector>

namespace steeplejack::ecs
{
struct DrawItem
{
    entt::entity entity{entt::null};
    MaterialId material{0};
    GeometryId geometry{0};
    const DrawRange* ranges{nullptr};
    std::size_t range_count{0};
};

inline std::vector<DrawItem> build_draw_list(entt::registry& reg)
{
    std::vector<DrawItem> result;
    auto group = reg.group<Transform>(entt::get<RenderMesh, MaterialRef>);
    result.reserve(group.size());

    for (auto e : group)
    {
        const auto& rm = group.get<RenderMesh>(e);
        const auto& mat = group.get<MaterialRef>(e);
        if (rm.draws.empty())
            continue;

        DrawItem item{};
        item.entity = e;
        item.material = mat.material;
        item.geometry = rm.geometry;
        item.ranges = rm.draws.data();
        item.range_count = rm.draws.size();
        result.emplace_back(item);
    }
    return result;
}
} // namespace steeplejack::ecs

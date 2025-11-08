// Geometry store for ECS (minimal)
#pragma once

#include <cstdint>
#include <unordered_map>

#include "ecs/components.h"

namespace steeplejack
{
class GraphicsBuffers;
}

namespace steeplejack::ecs
{
struct Geometry
{
    // For now, geometry is backed by the shared GraphicsBuffers bound by the engine.
    // Future: add per-geometry vertex/index buffer handles.
};

class GeometryStore
{
  private:
    std::unordered_map<GeometryId, Geometry> m_items;
    GeometryId m_next{1};

  public:
    GeometryStore() = default;

    GeometryId create()
    {
        GeometryId id = m_next++;
        m_items.emplace(id, Geometry{});
        return id;
    }

    const Geometry* get(GeometryId id) const
    {
        auto it = m_items.find(id);
        return (it == m_items.end()) ? nullptr : &it->second;
    }
};
} // namespace steeplejack::ecs


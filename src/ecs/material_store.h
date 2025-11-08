// Material store for ECS (minimal bridge to engine Material)
#pragma once

#include <cstdint>
#include <unordered_map>

#include "ecs/components.h"

namespace steeplejack
{
class Material;
}

namespace steeplejack::ecs
{
class MaterialStore
{
  private:
    std::unordered_map<MaterialId, Material*> m_items;
    MaterialId m_next{1};

  public:
    MaterialId add(Material* material)
    {
        MaterialId id = m_next++;
        m_items.emplace(id, material);
        return id;
    }

    Material* get(MaterialId id) const
    {
        auto it = m_items.find(id);
        return (it == m_items.end()) ? nullptr : it->second;
    }
};
} // namespace steeplejack::ecs


#pragma once

#include "ecs/types.h"

#include <entt/entt.hpp>

namespace steeplejack::ecs
{
class Registry
{
  public:
    using Underlying = entt::registry;

    Registry() = default;

    EntityId create()
    {
        const auto entity = m_registry.create();
        return static_cast<EntityId>(static_cast<entt::id_type>(entity));
    }

    void destroy(EntityId id)
    {
        const entt::entity e{id};
        if (m_registry.valid(e))
        {
            m_registry.destroy(e);
        }
    }

    [[nodiscard]] bool valid(EntityId id) const
    {
        return m_registry.valid(entt::entity{id});
    }

    template <typename Component, typename... Args> Component& emplace(EntityId id, Args&&... args)
    {
        return m_registry.emplace<Component>(entt::entity{id}, std::forward<Args>(args)...);
    }

    template <typename Component> void remove(EntityId id)
    {
        m_registry.remove<Component>(entt::entity{id});
    }

    template <typename Component> [[nodiscard]] bool has(EntityId id) const
    {
        return m_registry.any_of<Component>(entt::entity{id});
    }

    template <typename Component> Component& get(EntityId id)
    {
        return m_registry.get<Component>(entt::entity{id});
    }

    template <typename Component> const Component& get(EntityId id) const
    {
        return m_registry.get<Component>(entt::entity{id});
    }

    template <typename... Components> auto view()
    {
        return m_registry.view<Components...>();
    }

    template <typename... Components> auto view() const
    {
        return m_registry.view<Components...>();
    }

    Underlying& raw()
    {
        return m_registry;
    }
    [[nodiscard]] const Underlying& raw() const
    {
        return m_registry;
    }

  private:
    Underlying m_registry;
};
} // namespace steeplejack::ecs

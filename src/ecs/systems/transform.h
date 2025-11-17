// Transform and hierarchy systems (scaffold)
#pragma once

#include "ecs/components.h"

#include <entt/entt.hpp>

namespace steeplejack::ecs
{
inline glm::mat4 compose_trs(const Transform& t)
{
    return glm::translate(glm::mat4(1.0f), t.t) * glm::mat4_cast(t.r) * glm::scale(glm::mat4(1.0f), t.s);
}

// Attach/detach helpers for Relationship
inline void attach(entt::registry& reg, entt::entity parent, entt::entity child)
{
    auto& cr = reg.get_or_emplace<Relationship>(child);

    // If already attached, detach first
    if (cr.parent != entt::null)
    {
        // detach
        auto& pr = reg.get<Relationship>(cr.parent);
        if (cr.prev != entt::null)
        {
            reg.get<Relationship>(cr.prev).next = cr.next;
        }
        else
        {
            pr.first = cr.next;
        }
        if (cr.next != entt::null)
        {
            reg.get<Relationship>(cr.next).prev = cr.prev;
        }
    }

    cr.parent = parent;
    cr.prev = entt::null;
    cr.next = entt::null;

    auto& pr = reg.get_or_emplace<Relationship>(parent);
    cr.next = pr.first;
    if (pr.first != entt::null)
    {
        reg.get<Relationship>(pr.first).prev = child;
    }
    pr.first = child;
    cr.depth = pr.depth + 1;

    // mark transform dirty
    if (auto* t = reg.try_get<Transform>(child))
    {
        t->dirty = true;
    }
}

inline void detach(entt::registry& reg, entt::entity child)
{
    if (!reg.any_of<Relationship>(child))
        return;
    auto& cr = reg.get<Relationship>(child);

    if (cr.parent != entt::null)
    {
        auto& pr = reg.get<Relationship>(cr.parent);
        if (cr.prev != entt::null)
        {
            reg.get<Relationship>(cr.prev).next = cr.next;
        }
        else
        {
            pr.first = cr.next;
        }
        if (cr.next != entt::null)
        {
            reg.get<Relationship>(cr.next).prev = cr.prev;
        }
    }

    cr.parent = entt::null;
    cr.prev = entt::null;
    cr.next = entt::null;
    cr.depth = 0;

    // mark transform dirty
    if (auto* t = reg.try_get<Transform>(child))
    {
        t->dirty = true;
    }
}

inline void update_local(Transform& t)
{
    if (t.dirty)
    {
        t.local = compose_trs(t);
    }
}

inline void update_world_recursive(entt::registry& reg, entt::entity e, const glm::mat4& parent_world)
{
    auto& t = reg.get<Transform>(e);
    update_local(t);
    t.world = parent_world * t.local;
    t.dirty = false;

    if (auto* rel = reg.try_get<Relationship>(e))
    {
        auto child = rel->first;
        while (child != entt::null)
        {
            update_world_recursive(reg, child, t.world);
            child = reg.get<Relationship>(child).next;
        }
    }
}

inline void update_transforms(entt::registry& reg)
{
    // Process roots: entities with Transform and either no Relationship or parent == null
    auto view = reg.view<Transform, Relationship>();
    auto roots = std::vector<entt::entity>{};
    roots.reserve(static_cast<std::size_t>(view.size_hint()));
    for (auto [e, t, r] : view.each())
    {
        if (r.parent == entt::null)
        {
            roots.push_back(e);
        }
    }

    // Also catch standalone transforms without Relationship
    auto lone_view = reg.view<Transform>(entt::exclude<Relationship>);
    for (auto e : lone_view)
    {
        auto& t = reg.get<Transform>(e);
        update_local(t);
        t.world = t.local;
        t.dirty = false;
    }

    for (auto e : roots)
    {
        auto& t = reg.get<Transform>(e);
        update_local(t);
        t.world = t.local;
        t.dirty = false;
        update_world_recursive(reg, e, glm::mat4(1.0f));
    }
}

} // namespace steeplejack::ecs

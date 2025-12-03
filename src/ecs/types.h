#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace steeplejack::ecs
{
using EntityId = entt::entity;

constexpr EntityId kInvalidEntity = entt::null;
} // namespace steeplejack::ecs

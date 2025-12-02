#pragma once

#include <cstdint>

namespace steeplejack::ecs
{
using EntityId = std::uint32_t;

constexpr EntityId kInvalidEntity = 0;
} // namespace steeplejack::ecs

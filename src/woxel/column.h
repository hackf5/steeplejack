#pragma once

#include "woxel/woxel.h"

namespace steeplejack
{
namespace woxel
{
// Minimal column spec to evaluate the grid → mesh flow
struct ColumnSpec
{
    float inner_radius = 3.0f; // r0
    std::int32_t layers = 8;   // number of vertical layers
};
} // namespace woxel
} // namespace steeplejack


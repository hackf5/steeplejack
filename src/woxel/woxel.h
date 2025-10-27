#pragma once

#include "woxel/math.h"

#include <cstdint>
#include <cmath>

namespace steeplejack
{
namespace woxel
{
struct WoxelGrid
{
    // Angular sectors and fixed radius
    std::int32_t num_sectors = 64; // Nθ

    // Vertical height (dy) and radial thickness (dz) per woxel
    float dy = 1.0f; // height per layer (Z-up)
    float dz = 0.5f; // ring thickness

    // Derived angular step
    constexpr float dtheta() const noexcept
    {
        return k_two_pi / static_cast<float>(num_sectors);
    }

    // Index/coordinate helpers
    constexpr float theta_of(std::int32_t i_theta) const noexcept
    {
        return static_cast<float>(i_theta) * dtheta();
    }

    constexpr float z_of(std::int32_t i_z) const noexcept
    {
        return static_cast<float>(i_z) * dy;
    }

    constexpr std::int32_t wrap_theta_index(std::int32_t i_theta) const noexcept
    {
        const std::int32_t n = num_sectors;
        const std::int32_t m = ((i_theta % n) + n) % n;
        return m;
    }

    std::int32_t theta_index_of(float theta) const noexcept
    {
        const float t = wrap_angle(theta);
        const float f = t / dtheta();
        const std::int32_t i = static_cast<std::int32_t>(std::floor(f));
        return wrap_theta_index(i);
    }

    std::int32_t z_index_of(float z) const noexcept
    {
        return static_cast<std::int32_t>(std::floor(z / dy));
    }
};

// Discrete cell indices in (theta, z)
struct GridIndex
{
    std::int32_t i_theta = 0; // [0, sectors)
    std::int32_t i_z = 0;     // vertical layers

    bool operator==(const GridIndex& o) const noexcept
    {
        return i_theta == o.i_theta && i_z == o.i_z;
    }
};
} // namespace woxel
} // namespace steeplejack

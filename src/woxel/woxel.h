#pragma once

#include <cstdint>
#include <tuple>

#include <glm/glm.hpp>

#include "woxel/math.h"

namespace steeplejack
{
// Global parameters describing the cylindrical grid discretization
struct WoxelSpaceSpec
{
    // Angular sectors and base radius
    std::int32_t sectors = 64;   // Nθ
    float base_radius = 5.0f;    // R0

    // Steps (derived when zero)
    float dtheta = 0.0f;         // Δθ = 2π / sectors
    float hz = 1.0f;             // vertical step per layer
    float dr = 0.25f;            // radial layer step when Lr > 1

    // Radial layers (PoC default 1)
    std::int32_t radial_layers = 1; // Lr

    void finalize()
    {
        if (dtheta == 0.0f && sectors > 0)
        {
            dtheta = k_two_pi / static_cast<float>(sectors);
        }
    }
};

// Discrete cell indices in (theta, z, radial)
struct WoxelIndex
{
    std::int32_t i_theta = 0; // [0, sectors)
    std::int32_t i_z = 0;     // vertical layers
    std::int32_t i_r = 0;     // [0, radial_layers)

    bool operator==(const WoxelIndex &o) const
    { return i_theta == o.i_theta && i_z == o.i_z && i_r == o.i_r; }
};

// Continuous coordinates in cylindrical (θ, r, z)
struct WoxelCoord
{
    float theta = 0.0f; // radians, wraps mod 2π
    float r = 0.0f;     // world units
    float z = 0.0f;     // world units

    glm::vec3 to_cartesian() const { return woxel_to_cartesian(theta, r, z); }
};

// Conversions between index and continuous coordinates
inline float theta_of(const WoxelSpaceSpec &spec, std::int32_t i_theta)
{
    return (static_cast<float>(i_theta) * spec.dtheta);
}

inline float r_of(const WoxelSpaceSpec &spec, std::int32_t i_r)
{
    return spec.base_radius + static_cast<float>(i_r) * spec.dr;
}

inline float z_of(const WoxelSpaceSpec &spec, std::int32_t i_z)
{
    return static_cast<float>(i_z) * spec.hz;
}

inline std::int32_t wrap_theta_index(const WoxelSpaceSpec &spec, std::int32_t i_theta)
{
    const std::int32_t n = spec.sectors;
    const std::int32_t m = ((i_theta % n) + n) % n;
    return m;
}

inline std::int32_t i_theta_of(const WoxelSpaceSpec &spec, float theta)
{
    const float t = wrap_angle(theta);
    const float f = t / spec.dtheta;
    const std::int32_t i = static_cast<std::int32_t>(std::floor(f));
    return wrap_theta_index(spec, i);
}

inline std::int32_t i_z_of(const WoxelSpaceSpec &spec, float z)
{
    return static_cast<std::int32_t>(std::floor(z / spec.hz));
}

inline std::int32_t i_r_of(const WoxelSpaceSpec &spec, float r)
{
    const float offset = r - spec.base_radius;
    return static_cast<std::int32_t>(std::floor(offset / spec.dr));
}
}


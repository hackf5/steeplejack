#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "woxel/woxel.h"
#include "woxel/math.h"

namespace steeplejack
{
struct ActorState
{
    // Continuous cylindrical pose
    float theta = 0.0f; // radians
    float r = 0.0f;     // world units
    float z = 0.0f;     // world units

    // Velocities in local frame (tangent, normal, up)
    glm::vec3 v_tnu { 0.0f }; // {vt, vn, vz} in (t, n, u)

    glm::vec3 position_cartesian() const { return woxel_to_cartesian(theta, r, z); }

    OrthonormalFrame frame() const { return frame_at_theta(theta); }

    void wrap_theta() { theta = wrap_angle(theta); }
};
}


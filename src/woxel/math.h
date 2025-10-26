#pragma once

#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace steeplejack
{
constexpr float k_two_pi = glm::two_pi<float>();

inline float wrap_angle(float theta)
{
    // Wrap into [0, 2π)
    float t = std::fmod(theta, k_two_pi);
    if (t < 0.0f) t += k_two_pi;
    return t;
}

inline glm::vec3 woxel_to_cartesian(float theta, float r, float z)
{
    float c, s;
    sincosf(theta, &s, &c);
    return { r * c, r * s, z };
}

struct OrthonormalFrame
{
    // Tangent (increasing theta), Normal (outward radial), Up (+Z)
    glm::vec3 t; // (-sin θ, cos θ, 0)
    glm::vec3 n; // ( cos θ, sin θ, 0)
    glm::vec3 u; // ( 0,      0,     1)
};

inline OrthonormalFrame frame_at_theta(float theta)
{
    float c, s;
    sincosf(theta, &s, &c);
    return { glm::vec3(-s, c, 0.0f), glm::vec3(c, s, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
}
}

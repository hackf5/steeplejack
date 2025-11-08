#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

#include "types.glsl"

// Standard Lambert diffuse spotlight (soft cone + quadratic attenuation)
vec3 lambertDiffuse(in Spot light, in vec3 N, in vec3 worldPos, in vec3 baseColor) {
    // Ensure all vectors are normalized in the space we light in (world)
    vec3 Nn   = normalize(N);
    vec3 Ls   = normalize(light.position - worldPos);   // to-light
    vec3 Lf   = -Ls;                                     // from-light (beam direction)
    vec3 Dfwd = normalize(light.direction);              // light's forward

    // Diffuse term
    float NdotL = max(dot(Nn, Ls), 0.0);

    // Spotlight cone with smooth falloff
    float cosAng = dot(Dfwd, Lf);
    float innerC = light.innerCos;
    float outerC = light.outerCos;
    // Guard against swapped or equal cones
    float cone = smoothstep(min(outerC, innerC), max(outerC, innerC), cosAng);

    // Distance attenuation (quadratic-like)
    float dist  = length(worldPos - light.position);
    float range = max(light.range, 1e-3);
    float r     = dist / range;
    float att   = 1.0 / (1.0 + r * r);

    return baseColor * light.color * (light.intensity * NdotL * cone * att);
}

#endif

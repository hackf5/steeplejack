#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

#include "types.glsl"

// Standard Lambert diffuse spotlight (soft cone + quadratic attenuation)
vec3 lambertDiffuse(in Spot light, in vec3 N, in vec3 worldPos, in vec3 baseColor) {
    vec3 L = normalize(light.position - worldPos);
    float NdotL = max(dot(N, L), 0.0);

    float cosAng = dot(normalize(-light.direction), L);
    float innerC = light.innerCos;
    float outerC = light.outerCos;

    float cone = clamp((cosAng - outerC) / max(innerC - outerC, 1e-5), 0.0, 1.0);

    float dist = length(light.position - worldPos);
    float range = light.range;
    float att = 1.0 / (1.0 + (dist / max(range, 1e-3)) * (dist / max(range, 1e-3)));

    return baseColor * light.color * (light.intensity * NdotL * cone * att);
}

#endif

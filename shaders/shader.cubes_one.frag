#version 450

layout(binding = 2) uniform sampler2D inSampler;  // baseColor
layout(binding = 4) uniform sampler2D inNormal;   // normal (unused)
layout(binding = 5) uniform sampler2D inMR;       // metallic-roughness (unused)
layout(binding = 6) uniform sampler2D inEmissive; // emissive

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inWorldNormal;

layout(location = 0) out vec4 outColor;

layout(binding = 3) uniform MaterialParams {
    vec4 baseColorFactor;
};

const int MAX_SPOTS = 8;

struct Spot {
    vec4 posIntensity;   // xyz = position, w = intensity
    vec4 dirInnerCos;    // xyz = direction, w = innerCos
    vec4 colorOuterCos;  // rgb = color,   w = outerCos
    vec4 rangePad;       // x = range
};

layout(binding = 7) uniform SceneLights {
    vec3 ambientColor;  float ambientIntensity;
    vec4 spotPosIntensity;   // xyz pos, w intensity
    vec4 spotDirInnerCos;    // xyz dir, w innerCos
    vec4 spotColorOuterCos;  // rgb color, w outerCos
    vec4 spotRangePad;       // x range
};

void main() {
    vec4 baseCol = inColor * texture(inSampler, inUV) * baseColorFactor;
    vec3 emissiveCol = texture(inEmissive, inUV).rgb;
    vec3 ambient = baseCol.rgb * ambientColor * ambientIntensity;

    vec3 N = normalize(inWorldNormal);
    vec3 L = normalize(spotPosIntensity.xyz - inWorldPos);
    float NdotL = max(dot(N, L), 0.0);
    float cosAng = dot(normalize(-spotDirInnerCos.xyz), L);
    float innerC = spotDirInnerCos.w;
    float outerC = spotColorOuterCos.w;
    float cone = clamp((cosAng - outerC) / max(innerC - outerC, 1e-5), 0.0, 1.0);
    float dist = length(spotPosIntensity.xyz - inWorldPos);
    float range = spotRangePad.x;
    float att = 1.0 / (1.0 + (dist / max(range, 1e-3)) * (dist / max(range, 1e-3)));
    float intensity = spotPosIntensity.w;
    vec3 scolor = spotColorOuterCos.xyz;
    vec3 diffuse = baseCol.rgb * scolor * (intensity * NdotL * cone * att);

    outColor = vec4(ambient + diffuse + emissiveCol, baseCol.a);
}

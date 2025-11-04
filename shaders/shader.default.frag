#version 460

#extension GL_GOOGLE_include_directive : require

#include "types.glsl"
#include "lighting.glsl"

const int kMaxSpots = 8;

layout(binding = 2) uniform sampler2D inSampler;  // baseColor
layout(binding = 4) uniform sampler2D inNormal;   // normal (unused)
layout(binding = 5) uniform sampler2D inMR;       // metallic-roughness (unused)
layout(binding = 6) uniform sampler2D inEmissive; // emissive

layout(binding = 3, std140) uniform MaterialParams {
    vec4 baseColorFactor;
};

layout(binding = 7, std140) uniform SceneLights {
    vec3 ambientColor;  float ambientIntensity;
    Spot spots[kMaxSpots];
};

layout (binding = 9, std140) uniform ShadowParams {
    mat4 lightViewProj[kMaxSpots];
};

layout(binding = 8) uniform sampler2DArray inShadowMaps; // shadow maps (unused)


layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inWorldNormal;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 baseCol = texture(inSampler, inUV) * baseColorFactor;
    vec3 emissiveCol = texture(inEmissive, inUV).rgb;
    vec3 ambient = baseCol.rgb * ambientColor * ambientIntensity;

    vec3 N = normalize(inWorldNormal);
    vec3 diffuseSum = vec3(0.0);
    for (int i = 0; i < kMaxSpots; ++i) {
        if (spots[i].enable) {
            diffuseSum += lambertDiffuse(spots[i], N, inWorldPos, baseCol.rgb);
        }
    }

    outColor = vec4(ambient + diffuseSum + emissiveCol, baseCol.a);
}

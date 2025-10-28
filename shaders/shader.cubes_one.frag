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
    vec3 position;  float intensity;
    vec3 direction; float innerCos;
    vec3 color;     float outerCos;
    float range;    vec3 _pad;
};

layout(binding = 7) uniform SceneLights {
    vec3 ambientColor;  float ambientIntensity;
    int spotCount;      vec3 _padCount;
    Spot spots[MAX_SPOTS];
};

void main() {
    vec4 baseCol = inColor * texture(inSampler, inUV) * baseColorFactor;
    vec3 emissiveCol = texture(inEmissive, inUV).rgb;
    vec3 ambient = baseCol.rgb * ambientColor * ambientIntensity;

    vec3 N = normalize(inWorldNormal);
    vec3 diffuseSum = vec3(0.0);
    for (int i = 0; i < spotCount && i < MAX_SPOTS; ++i) {
        vec3 L = normalize(spots[i].position - inWorldPos);
        float NdotL = max(dot(N, L), 0.0);
        float cosAng = dot(normalize(-spots[i].direction), L);
        float cone = clamp((cosAng - spots[i].outerCos) / max(spots[i].innerCos - spots[i].outerCos, 1e-5), 0.0, 1.0);
        float dist = length(spots[i].position - inWorldPos);
        float att = 1.0 / (1.0 + (dist / max(spots[i].range, 1e-3)) * (dist / max(spots[i].range, 1e-3)));
        diffuseSum += baseCol.rgb * spots[i].color * (spots[i].intensity * NdotL * cone * att);
    }

    outColor = vec4(ambient + diffuseSum + emissiveCol, baseCol.a);
}

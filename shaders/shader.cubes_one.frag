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

struct Spot {
    vec3 position;  float intensity;
    vec3 direction; float innerCos;
    vec3 color;     float outerCos;
    float range;    vec3 _pad; // std140 alignment
};

layout(binding = 7) uniform SceneLights {
    vec3 ambientColor;  float ambientIntensity;
    Spot spots[1];
};

void main() {
    vec4 baseCol = texture(inSampler, inUV) * baseColorFactor;
    vec3 emissiveCol = texture(inEmissive, inUV).rgb;
    vec3 ambient = baseCol.rgb * ambientColor * ambientIntensity;

    vec3 N = normalize(inWorldNormal);
    vec3 L = normalize(spots[0].position - inWorldPos);
    float NdotL = max(dot(N, L), 0.0);
    float cosAng = dot(normalize(-spots[0].direction), L);
    float innerC = spots[0].innerCos;
    float outerC = spots[0].outerCos;
    float cone = clamp((cosAng - outerC) / max(innerC - outerC, 1e-5), 0.0, 1.0);
    float dist = length(spots[0].position - inWorldPos);
    float range = spots[0].range;
    float att = 1.0 / (1.0 + (dist / max(range, 1e-3)) * (dist / max(range, 1e-3)));
    float intensity = spots[0].intensity;
    vec3 scolor = spots[0].color;
    vec3 diffuse = baseCol.rgb * scolor * (intensity * NdotL * cone * att);

    outColor = vec4(ambient + diffuse + emissiveCol, baseCol.a);
}

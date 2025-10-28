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

layout(binding = 7) uniform SceneLights {
    vec3 ambientColor;  float ambientIntensity;
    vec3 spotPosition;  float spotIntensity;
    vec3 spotDirection; float spotInnerCos;
    vec3 spotColor;     float spotOuterCos;
    float spotRange;    vec3 _pad0;
};

void main() {
    vec4 baseCol = inColor * texture(inSampler, inUV) * baseColorFactor;
    vec3 emissiveCol = texture(inEmissive, inUV).rgb;
    vec3 ambient = baseCol.rgb * ambientColor * ambientIntensity;

    // Spotlight (Lambert diffuse + smooth cone + distance attenuation)
    vec3 N = normalize(inWorldNormal);
    vec3 L = normalize(spotPosition - inWorldPos);
    float NdotL = max(dot(N, L), 0.0);
    float cosAng = dot(normalize(-spotDirection), L);
    float cone = clamp((cosAng - spotOuterCos) / max(spotInnerCos - spotOuterCos, 1e-5), 0.0, 1.0);
    float dist = length(spotPosition - inWorldPos);
    float att = 1.0 / (1.0 + (dist / max(spotRange, 1e-3)) * (dist / max(spotRange, 1e-3)));
    vec3 diffuse = baseCol.rgb * spotColor * (spotIntensity * NdotL * cone * att);

    outColor = vec4(ambient + diffuse + emissiveCol, baseCol.a);
}

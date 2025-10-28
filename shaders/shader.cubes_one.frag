#version 450

layout(binding = 2) uniform sampler2D inSampler;  // baseColor
layout(binding = 4) uniform sampler2D inNormal;   // normal (unused)
layout(binding = 5) uniform sampler2D inMR;       // metallic-roughness (unused)
layout(binding = 6) uniform sampler2D inEmissive; // emissive (unused)

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

layout(binding = 3) uniform MaterialParams {
    vec4 baseColorFactor;
};

layout(binding = 7) uniform SceneLights {
    vec3 ambientColor;
    float ambientIntensity;
};

void main() {
    vec4 baseCol = inColor * texture(inSampler, inUV) * baseColorFactor;
    vec3 emissiveCol = texture(inEmissive, inUV).rgb;
    vec3 ambient = baseCol.rgb * ambientColor * ambientIntensity;
    outColor = vec4(ambient + emissiveCol, baseCol.a);
}

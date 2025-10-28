#version 450

layout(binding = 2) uniform sampler2D inSampler;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

layout(binding = 3) uniform MaterialParams {
    vec4 baseColorFactor;
};

void main() {
    outColor = inColor * texture(inSampler, inUV) * baseColorFactor;
}

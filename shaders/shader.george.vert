#version 450

layout(binding = 0) uniform Camera {
    mat4 proj;
    mat4 view;
} camera;

layout(binding = 1) uniform Mesh {
    mat4 model;
} mesh;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;

void main() {
    gl_Position = camera.proj * camera.view * mesh.model * vec4(inPosition, 1.0);
    outUV = inUV;
    outColor = inColor;
}
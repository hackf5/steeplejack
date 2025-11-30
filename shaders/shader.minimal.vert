#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;

layout(binding = 0) uniform Camera {
    mat4 proj;
    mat4 view;
} camera;

layout(binding = 1) uniform Model {
    mat4 model;
} model;

layout(location = 0) out vec2 fragUV;

void main()
{
    fragUV = inUV;
    gl_Position = camera.proj * camera.view * model.model * vec4(inPos, 1.0);
}

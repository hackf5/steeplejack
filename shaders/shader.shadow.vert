#version 460

layout(binding = 0, std140) uniform Light {
    mat4 viewProj;
} light;

layout(binding = 1, std140) uniform Model {
    mat4 model;
};

layout(location = 0) in vec3 inPosition;

void main() {
    gl_Position = light.viewProj * model * vec4(inPosition, 1.0);
}
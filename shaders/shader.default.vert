#version 460

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
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;
layout(location = 2) out vec3 outWorldPos;
layout(location = 3) out vec3 outWorldNormal;

void main() {
    vec4 worldPos = mesh.model * vec4(inPosition, 1.0);
    gl_Position = camera.proj * camera.view * worldPos;

    outUV = inUV;
    outColor = inColor;
    outWorldPos = worldPos.xyz;
    // Normal transform: inverse-transpose of model's upper-left 3x3
    mat3 normalMat = mat3(transpose(inverse(mesh.model)));
    outWorldNormal = normalize(normalMat * inNormal);
}

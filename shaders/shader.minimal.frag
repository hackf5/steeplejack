#version 460

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

void main()
{
    // Simple gradient based on UV for visibility
    outColor = vec4(fragUV, 0.5, 1.0);
}

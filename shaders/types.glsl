#ifndef TYPES_GLSL
#define TYPES_GLSL

struct Spot {
    vec3 position;  float intensity;
    vec3 direction; float innerCos;
    vec3 color;     float outerCos;
    float range;    bool enable;   vec2 _pad0;
};

#endif // TYPES_GLSL
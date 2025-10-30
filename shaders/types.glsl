#ifndef TYPES_GLSL
#define TYPES_GLSL

struct Spot {
    vec3 position;  float intensity;
    vec3 direction; float innerCos;
    vec3 color;     float outerCos;
    vec3 _pad;      float range;
};

#endif // TYPES_GLSL
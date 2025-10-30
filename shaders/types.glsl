#ifndef TYPES_GLSL
#define TYPES_GLSL

struct Spot {
    vec3 position;  float intensity;
    vec3 direction; float innerCos;
    vec3 color;     float outerCos;
    float range;    vec3 _pad; // std140 alignment
};

#endif // TYPES_GLSL
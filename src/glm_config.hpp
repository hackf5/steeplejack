// glm_config.hpp
#pragma once
#ifdef GLM_VERSION
#error "GLM was included before glm_config.hpp."
#endif

// #ifndef GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
// #endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

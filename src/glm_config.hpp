// glm_config.hpp
#pragma once
#pragma once
#ifdef GLM_VERSION
#error "GLM was included before glm_config.hpp. Include this header first (or define the macro via the build)."
#endif

#ifndef GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#ifndef STEEPLEJACK_GLM
#define STEEPLEJACK_GLM
// static_assert(alignof(glm::vec3) == 16, "vec3 must be 16-aligned");
// static_assert(sizeof(glm::vec3)  == 16, "vec3 must be 16 bytes");
#endif

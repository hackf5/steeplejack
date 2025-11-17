// glm_config.hpp
#pragma once
#ifdef GLM_VERSION
#error "GLM was included before glm_config.hpp."
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef INTERFACE_GLSL
#define INTERFACE_GLSL

#ifdef __cplusplus
#include <glm/mat4x4.hpp>

namespace R1::GLSL {
using uint = unsigned;
using mat4 = glm::mat4;
#endif // __cplusplus

const uint transform_ssbo_binding = 0;
const uint global_ubo_binding = 1;

#define GLOBAL_UBO_DEFINITION(name) name {\
    mat4 proj_view;\
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // INTERFACE_GLSL

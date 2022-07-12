#ifndef INTERFACE_GLSL
#define INTERFACE_GLSL

#ifdef __cplusplus
#include <glm/mat4x4.hpp>

#define DEFINE_UINT using uint = unsigned;
#define DEFINE_VEC3 using vec3 = glm::vec3;
#define DEFINE_MAT3 using mat3 = glm::mat3;
#define DEFINE_MAT4 using mat4 = glm::mat4;

#define ALIGN_AS(arg) alignas(arg)
#elif GL_core_profile
#define DEFINE_UINT
#define DEFINE_VEC3
#define DEFINE_MAT3
#define DEFINE_MAT4

#define ALIGN_AS(arg)
#endif // __cplusplus

#define GLOBAL_UBO_DEFINITION(type, name) \
type ALIGN_AS(64) name { \
    mat4 proj_view; \
    vec3 camera_pos; \
}

#define DEFINE_GLSL_INTERFACE_TYPES \
DEFINE_UINT \
DEFINE_VEC3 \
DEFINE_MAT3 \
DEFINE_MAT4 \
struct InstanceMatrices { \
    mat4 model; \
    mat3 normal; \
}; \
GLOBAL_UBO_DEFINITION(struct, GlobalUBO); \
\
const uint transform_ssbo_binding = 0; \
const uint global_ubo_binding = 1; \
// DEFINE_GLSL_INTERFACE_TYPES

#if GL_core_profile
DEFINE_GLSL_INTERFACE_TYPES
#endif

#endif // INTERFACE_GLSL

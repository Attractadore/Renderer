#version 450
#extension GL_EXT_scalar_block_layout: require
#include "Interface.glsl"

layout(location = 0) in vec3 pos;

layout(set = 0, binding = transform_ssbo_binding, std430)
restrict readonly buffer TransformSSBO {
    mat4[] model;
};

layout(set = 0, binding = global_ubo_binding, std430)
uniform GLOBAL_UBO_DEFINITION(GlobalUBO);

void main() {
    gl_Position = proj_view * model[gl_InstanceIndex] * vec4(pos, 1.0f);
}

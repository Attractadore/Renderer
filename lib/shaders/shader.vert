#version 450
#extension GL_EXT_scalar_block_layout: require
#include "Interface.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec3 frag_position;
layout(location = 1) out vec3 frag_normal;

layout(set = 0, binding = transform_ssbo_binding, scalar)
restrict readonly buffer TransformSSBO {
    InstanceMatrices[] transforms;
};

layout(set = 0, binding = global_ubo_binding, scalar)
GLOBAL_UBO_DEFINITION(uniform, UBO);

void main() {
    InstanceMatrices mats = transforms[gl_InstanceIndex];

    vec4 global_position = mats.model * vec4(position, 1.0f);
    frag_position = global_position.xyz;
    frag_normal = mats.normal * normal;
    gl_Position = proj_view * global_position;
}

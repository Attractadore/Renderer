#version 450
#extension GL_EXT_scalar_block_layout: require
#include "Interface.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 frag_normal;

layout(location = 0) out vec4 color;

layout(set = 0, binding = global_ubo_binding, scalar)
GLOBAL_UBO_DEFINITION(uniform, UBO);

void main() {
    const vec3 light_color = vec3(1.0f);
    const vec3 diffuse_color = vec3(1.0f);
    const float shininess = 16.0f;

    vec3 normal = normalize(frag_normal);

    vec3 light_dir = vec3(0.0f, 0.0f, 1.0f);
    vec3 reflect_dir = reflect(-light_dir, normal);
    vec3 view_dir = normalize(camera_pos - position);

    float specular_intencity = pow(max(dot(reflect_dir, view_dir), 0.0f), shininess);
    float diffuse_intencity = max(dot(normal, light_dir), 0.0f);
    float ambient_intencity = 0.1f;
    float intencity = specular_intencity + diffuse_intencity + ambient_intencity;

    color = vec4(diffuse_color * light_color * intencity, 1.0f);
}

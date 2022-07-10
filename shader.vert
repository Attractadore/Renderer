#version 450
layout(location = 0) in vec3 pos;

layout(std430, binding = 0, set = 0) restrict readonly buffer TransformSSBO {
    mat4[] model;
};

void main() {
    gl_Position = model[gl_InstanceIndex] * vec4(pos, 1.0f);
}

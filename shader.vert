#version 450

const vec2 positions[] = vec2[](
    vec2( 0.0f,  0.5f),
    vec2( 0.5f, -0.5f),
    vec2(-0.5f, -0.5f)
);

void main() {
    vec2 pos = positions[gl_VertexIndex];
    gl_Position = vec4(pos, 0.0f, 1.0f);
}

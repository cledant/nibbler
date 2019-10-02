#version 410 core
layout (location = 0) in vec2 pos;
layout (location = 0) in vec3 color;

out VS_OUT {
    vec3 color;
} vs_out;

void main()
{
    gl_position = vec4(pos, 0.0, 1.0);
    vs_out.color = color;
}
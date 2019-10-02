#version 410 core

in GS_OUT {
    vec3 color;
} fs_in;

out vec4 color;

void main()
{
    color = (1.0, fs_in.color);
}

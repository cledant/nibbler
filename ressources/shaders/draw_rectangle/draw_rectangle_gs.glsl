#version 410 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out GS_OUT {
    vec3 color;
} gs_out;

uniform vec2    uniform_scale;

void main()
{
    gs_out.color = gs_in[0].color;
    gl_Position = gl_in[0].gl_Position + (vec4(uniform_scale, 0.0, 1.0) * vec4(-1.0, -1.0, 0.0, 0.0));
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + (vec4(uniform_scale, 0.0, 1.0) * vec4(1.0, -1.0, 0.0, 0.0));
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + (vec4(uniform_scale, 0.0, 1.0) * vec4(1.0, 1.0, 0.0, 0.0));
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + (vec4(uniform_scale, 0.0, 1.0) * vec4(1.0, 1.0, 0.0, 0.0));
    EmitVertex();
    EndPrimitive();
}

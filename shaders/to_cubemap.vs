#version 330 core
layout (location = 0) in vec3 a_position;

out vec3 vs_pos;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    vs_pos = 2.0f * a_position;
    gl_Position =  u_projection * u_view * vec4(vs_pos, 1.0);
}

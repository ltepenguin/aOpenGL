#version 330 core
layout (location = 0) in vec4 a_position;
// uniforms ----------------------------------------------------- //
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
// -------------------------------------------------------------- //
// output
out vec3 fs_worldPos;
out vec2 fs_uv;
// -------------------------------------------------------------- //
void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_position.xy, 0.0, 1.0);
    //gl_Position = u_model * vec4(a_position.xy, 0.0, 1.0);
    fs_uv = a_position.zw;
}

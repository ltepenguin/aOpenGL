#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
layout (location = 5) in vec3 a_materialID;
// uniforms ----------------------------------------------------- //
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
//uniform vec3 u_viewPosition;
//uniform vec3 u_lightDirection;
uniform mat4 u_lightSpace;
// -------------------------------------------------------------- //
// output
out vec2 fs_uv;
out vec3 fs_worldPos;
out vec3 fs_normal;
out vec3 fs_tangent;
out vec3 fs_bitangent;
out vec4 fs_lightSpacePos;
flat out int fs_materialID;
// -------------------------------------------------------------- //
void main()
{
    fs_uv = a_uv;
    fs_worldPos = vec3(u_model * vec4(a_position, 1.0));
    fs_normal = mat3(u_model) * a_normal;
    fs_tangent = mat3(u_model) * a_tangent;
    fs_bitangent = mat3(u_model) * a_bitangent;
    fs_lightSpacePos = u_lightSpace * vec4(fs_worldPos, 1.0);

    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
    fs_materialID = int(a_materialID.x);
}

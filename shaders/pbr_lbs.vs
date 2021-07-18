#version 330 core
const int MAX_JOINT_NUM = 100;
uniform mat4 u_lbs_joints[MAX_JOINT_NUM];

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
layout (location = 5) in vec3 a_materialID;
layout (location = 6) in vec4 a_lbs_jointIDs;
layout (location = 7) in vec4 a_lbs_weights;
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

void main()
{
    // Linear Blend Skining ========================================= //
    int jidx0 = int(a_lbs_jointIDs.x);
    int jidx1 = int(a_lbs_jointIDs.y);
    int jidx2 = int(a_lbs_jointIDs.z);
    int jidx3 = int(a_lbs_jointIDs.w);

    mat4 lbs_model = a_lbs_weights.x * u_lbs_joints[jidx0]
                   + a_lbs_weights.y * u_lbs_joints[jidx1]
                   + a_lbs_weights.z * u_lbs_joints[jidx2]
                   + a_lbs_weights.w * u_lbs_joints[jidx3];
    // ============================================================== //

    fs_uv = a_uv;
    fs_worldPos = vec3(lbs_model * vec4(a_position, 1.0));
    fs_normal = mat3(lbs_model) * a_normal;
    fs_tangent = mat3(lbs_model) * a_tangent;
    fs_bitangent = mat3(lbs_model) * a_bitangent;
    fs_lightSpacePos = u_lightSpace * vec4(fs_worldPos, 1.0);
    
    gl_Position = u_projection * u_view * lbs_model * vec4(a_position, 1.0);
    fs_materialID = int(a_materialID.x);
}

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

// uniforms
uniform bool u_use_lbs;
uniform mat4 u_model;
uniform mat4 u_lightSpace;

void main()
{
    if(u_use_lbs)
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

        gl_Position = u_lightSpace * lbs_model * vec4(a_position, 1.0);
    }
    else
    {
        gl_Position = u_lightSpace * u_model * vec4(a_position, 1.0);
    }
}

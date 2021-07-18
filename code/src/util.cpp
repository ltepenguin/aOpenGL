#include "aOpenGL/util.h"

namespace a::gl {

float safe_acos(float rad)
{
    rad = std::max(std::min(rad, 1.0f), -1.0f);
    return std::acos(rad);
}

Mat4 interpolate_rigid(const Mat4& trf1, const Mat4& trf2, float w1)
{
    Quat q1(trf1.block<3, 3>(0, 0));
    Quat q2(trf2.block<3, 3>(0, 0));
    Quat q = q2.slerp(w1, q1).normalized();

    Vec3 T1 = trf1.block<3, 1>(0, 3);
    Vec3 T2 = trf2.block<3, 1>(0, 3);
    Vec3 T = w1 * T1 + (1.0f - w1) * T2;
    
    return a::gl::to_mat4(q, T);
}

Mat4 project_to_ground(const Mat4& trf)
{
    Vec3 xaxis = trf.col(0).head<3>();
    xaxis.y() = 0.0f;
    xaxis.normalize();
    Vec3 zaxis = xaxis.cross(Vec3::UnitY());

    Mat4 projected = Mat4::Identity();
    projected.col(0).head<3>() = xaxis;
    projected.col(1).head<3>() = Vec3::UnitY();
    projected.col(2).head<3>() = zaxis;
    projected.col(3).head<3>() = trf.col(3).head<3>();
    projected.col(3).y() = 0.0f;

    return projected;
}

float get_angle(const Vec3& v1, const Vec3& v2)
{
    return a::gl::safe_acos(v1.dot(v2) / v1.norm() / v2.norm());
}

float get_sangle(const Vec3& v1, const Vec3& v2, const Vec3& upVec)
{
    return a::gl::get_angle(v1.cross(v2), upVec) < a::gl::to_rad(90.0) ? 
        a::gl::get_angle(v1, v2) : -a::gl::get_angle(v1, v2);
}

}
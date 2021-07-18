#include "aOpenGL/eigentype.h"

namespace a::gl {

Quat to_eigen(const glm::quat& q)
{
    return Quat(q.w, q.x, q.y, q.z);
}

Vec2i to_eigen(const glm::ivec2& v)
{
    return Vec2i(v.x, v.y);
}

Vec3i to_eigen(const glm::ivec3& v)
{
    return Vec3i(v.x, v.y, v.z);
}

Vec4i to_eigen(const glm::ivec4& v)
{
    return Vec4i(v.x, v.y, v.z, v.w);
}

Vec2  to_eigen(const glm::vec2& v)
{
    return Vec2(v.x, v.y);
}

Vec3  to_eigen(const glm::vec3& v)
{
    return Vec3(v.x, v.y, v.z);
}

Vec4  to_eigen(const glm::vec4& v)
{
    return Vec4(v.x, v.y, v.z, v.w);
}

Mat2  to_eigen(const glm::mat2& m)
{
    Mat2 M;
    M.col(0) = a::gl::to_eigen(m[0]);
    M.col(1) = a::gl::to_eigen(m[1]);
    return M;
}

Mat3  to_eigen(const glm::mat3& m)
{
    Mat3 M;
    M.col(0) = a::gl::to_eigen(m[0]);
    M.col(1) = a::gl::to_eigen(m[1]);
    M.col(2) = a::gl::to_eigen(m[2]);
    return M;
}

Mat4  to_eigen(const glm::mat4& m)
{
    Mat4 M;
    M.col(0) = a::gl::to_eigen(m[0]);
    M.col(1) = a::gl::to_eigen(m[1]);
    M.col(2) = a::gl::to_eigen(m[2]);
    M.col(3) = a::gl::to_eigen(m[3]);
    return M;
}

glm::quat  to_glm(const Quat& q)
{
    return glm::quat(q.w(), q.x(), q.y(), q.z());
}

glm::ivec2 to_glm(const Vec2i& v)
{
    return glm::ivec2(v.x(), v.y());
}

glm::ivec3 to_glm(const Vec3i& v)
{
    return glm::ivec3(v.x(), v.y(), v.z());
}

glm::ivec4 to_glm(const Vec4i& v)
{
    return glm::ivec4(v.x(), v.y(), v.z(), v.w());
}

glm::vec2  to_glm(const Vec2& v)
{
    return glm::vec2(v.x(), v.y());
}

glm::vec3  to_glm(const Vec3& v)
{
    return glm::vec3(v.x(), v.y(), v.z());
}

glm::vec4  to_glm(const Vec4& v)
{
    return glm::vec4(v.x(), v.y(), v.z(), v.w());
}

glm::mat2  to_glm(const Mat2& M)
{
    glm::mat2 m;
    m[0] = a::gl::to_glm((Vec2)M.col(0));
    m[1] = a::gl::to_glm((Vec2)M.col(1));
    return m;
}

glm::mat3  to_glm(const Mat3& M)
{
    glm::mat3 m;
    m[0] = a::gl::to_glm((Vec3)M.col(0));
    m[1] = a::gl::to_glm((Vec3)M.col(1));
    m[2] = a::gl::to_glm((Vec3)M.col(2));
    return m;
}

glm::mat4  to_glm(const Mat4& M)
{
    glm::mat4 m;
    m[0] = a::gl::to_glm((Vec4)M.col(0));
    m[1] = a::gl::to_glm((Vec4)M.col(1));
    m[2] = a::gl::to_glm((Vec4)M.col(2));
    m[3] = a::gl::to_glm((Vec4)M.col(3));
    return m;
}

Quat to_quat(const Mat3& M)
{
    return Quat(M);
}

Mat3 to_mat3(const Quat& q)
{
    return q.toRotationMatrix();
}

Mat4 to_mat4(const Quat& q)
{
    return a::gl::to_mat4(q.toRotationMatrix());
}

Mat4 to_mat4(const Mat3& M3)
{
    Mat4 M4 = Mat4::Identity();
    M4.block<3, 3>(0, 0) = M3;
    return M4;
}

Mat4 to_mat4(const Vec3& V3)
{
    Mat4 M4 = Mat4::Identity();
    M4.block<3, 1>(0, 3) = V3;
    return M4;
}

Mat4 to_mat4(const Quat& q, const Vec3& V)
{
    Mat4 M4 = Mat4::Identity();
    M4.block<3, 3>(0, 0) = a::gl::to_mat3(q);
    M4.block<3, 1>(0, 3) = V;
    return M4;
}

}
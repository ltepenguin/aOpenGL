#pragma once
#include <Eigen/Geometry>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

// Eigen
using VecNi = Eigen::VectorXi;
using VecN  = Eigen::VectorXf;
using Mat   = Eigen::MatrixXf;

// Anagle Axis
using AAxis = Eigen::AngleAxis<float>;

// Matrix Define
using MatBase = Eigen::MatrixBase<float>;

using Mat2  = Eigen::Matrix<float, 2, 2>;
using Mat22 = Eigen::Matrix<float, 2, 2>;
using Mat23 = Eigen::Matrix<float, 2, 3>;
using Mat24 = Eigen::Matrix<float, 2, 4>;
using Mat25 = Eigen::Matrix<float, 2, 5>;
using Mat26 = Eigen::Matrix<float, 2, 6>;

using Mat32 = Eigen::Matrix<float, 3, 2>;
using Mat33 = Eigen::Matrix<float, 3, 3>;
using Mat3  = Eigen::Matrix<float, 3, 3>;
using Mat34 = Eigen::Matrix<float, 3, 4>;
using Mat35 = Eigen::Matrix<float, 3, 5>;
using Mat36 = Eigen::Matrix<float, 3, 6>;

using Mat42 = Eigen::Matrix<float, 4, 2>;
using Mat43 = Eigen::Matrix<float, 4, 3>;
using Mat44 = Eigen::Matrix<float, 4, 4>;
using Mat4  = Eigen::Matrix<float, 4, 4>;
using Mat45 = Eigen::Matrix<float, 4, 5>;
using Mat46 = Eigen::Matrix<float, 4, 6>;

// Vector Define
using Vec2 = Eigen::Matrix<float, 2, 1>;
using Vec3 = Eigen::Matrix<float, 3, 1>;
using Vec4 = Eigen::Matrix<float, 4, 1>;
using Vec2i = Eigen::Vector2i;
using Vec3i = Eigen::Vector3i;
using Vec4i = Eigen::Vector4i;

// Quaternion
using Quat = Eigen::Quaternion<float>;

// shortcut
using vvec3 = std::vector<glm::vec3>;
using vquat = std::vector<glm::quat>;
using vQuat = std::vector<Quat>;
using vVec3 = std::vector<Vec3>;

namespace a::gl {

// eigen <--> glm
// todo: https://stackoverflow.com/questions/7351659/how-to-initialize-a-glmmat4-with-an-array

Quat  to_eigen(const glm::quat&);
Vec2i to_eigen(const glm::ivec2&);
Vec3i to_eigen(const glm::ivec3&);
Vec4i to_eigen(const glm::ivec4&);
Vec2  to_eigen(const glm::vec2&);
Vec3  to_eigen(const glm::vec3&);
Vec4  to_eigen(const glm::vec4&);
Mat2  to_eigen(const glm::mat2&);
Mat3  to_eigen(const glm::mat3&);
Mat4  to_eigen(const glm::mat4&);

glm::quat  to_glm(const Quat&);
glm::ivec2 to_glm(const Vec2i&);
glm::ivec3 to_glm(const Vec3i&);
glm::ivec4 to_glm(const Vec4i&);
glm::vec2  to_glm(const Vec2&);
glm::vec3  to_glm(const Vec3&);
glm::vec4  to_glm(const Vec4&);
glm::mat2  to_glm(const Mat2&);
glm::mat3  to_glm(const Mat3&);
glm::mat4  to_glm(const Mat4&);

Quat to_quat(const Mat3&);
Mat3 to_mat3(const Quat&);
Mat4 to_mat4(const Quat&);
Mat4 to_mat4(const Mat3&);
Mat4 to_mat4(const Vec3&);
Mat4 to_mat4(const Quat&, const Vec3&);

// std::vector<T> casting

template<typename T>
std::vector<Mat4> to_mat4(const std::vector<T>& eigens)
{
    int n = (int)eigens.size();
    std::vector<Mat4> m4s;
    m4s.reserve(n);
    for(int i = 0; i < n; ++i)
        m4s.push_back(std::move(a::gl::to_mat4(eigens.at(i))));
    return m4s;
}

template<typename srcT, typename tarT>
std::vector<tarT> to_eigen(const std::vector<srcT>& glms)
{
    int n = (int)glms.size();
    std::vector<tarT> eigens;
    eigens.reserve(n);
    for(int i = 0; i < n; ++i)
        eigens.push_back(std::move(a::gl::to_eigen(glms.at(i))));
    return eigens;
}

template<typename srcT, typename tarT>
std::vector<tarT> to_glm(const std::vector<srcT>& eigens)
{
    int n = (int)eigens.size();
    std::vector<tarT> glms;
    glms.reserve(n);
    for(int i = 0; i < n; ++i)
        glms.push_back(std::move(a::gl::to_glm(eigens.at(i))));
    return glms;
}

}
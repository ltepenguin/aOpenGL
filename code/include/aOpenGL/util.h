#pragma once
#include "eigentype.h"

namespace a::gl {

inline float to_rad(float deg) { return deg / 180.0f * EIGEN_PI; }
inline float to_deg(float rad) { return rad / EIGEN_PI * 180.0f; }
float safe_acos(float rad);

/**
 * @return interpolate two rigid transformation (translation and rotation only)
 */
Mat4 interpolate_rigid(const Mat4& trf1, const Mat4& trf2, float w1); 

Mat4 project_to_ground(const Mat4& trf);

/**
 * @return angle between v1 and v2
 */
float get_angle(const Vec3& v1, const Vec3& v2);

/**
 * @return signed angle between v1 and v2
 */
float get_sangle(const Vec3& v1, const Vec3& v2, const Vec3& upVec);

}
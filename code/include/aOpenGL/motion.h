#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include "aOpenGL/eigentype.h"

namespace a::gl {

/**
 * @brief pose
 * @since Thu Aug 27 2020
 */
struct Pose
{
    std::vector<Quat> local_rotations;
    Vec3              root_position;
};

/**
 * @brief name은 scene name과 같음
 */
struct Motion
{
    std::string name;
    std::vector<Pose> poses;
    float start_time;
    float end_time;
};

}
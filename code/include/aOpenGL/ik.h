#pragma once
#include <memory>
#include "eigentype.h"

namespace a::gl {

class Joint;
using spJoint = std::shared_ptr<Joint>;

/**
 * @brief simple two-bone IK
 * 
 * @param parent                 parent joint of child joint
 * @param child                  parent joint of end_effector joint
 * @param end_effector           end_effector
 * @param target                 desired position you want end_effector to be
 * @param use_opposite_dir       whether to use opposite direction of parent's rotation axis when the arm is fully extended or fully folded
 */
void two_bone_ik(spJoint parent, 
                 spJoint child, 
                 spJoint end_effector, 
                 Mat4 target, 
                 bool use_opposite_dir);

}
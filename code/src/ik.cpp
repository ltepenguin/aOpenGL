#include "aOpenGL/ik.h"
#include "aOpenGL/joint.h"
#include "aOpenGL/util.h"

namespace a::gl {

// http://theorangeduck.com/page/simple-two-joint
//
void two_bone_ik(spJoint parent, 
                 spJoint child, 
                 spJoint end_effector, 
                 Mat4 target, 
                 bool use_opposite_dir)
{   
    //** setting variables **//
    const float epsilon = 0.001f;

    //get world position
    Vec3 parent_worldPos = parent->world_pos();
    Vec3 child_worldPos = child->world_pos();
    Vec3 ee_worldPos = end_effector->world_pos();
    Vec3 target_worldPos = target.col(3).head<3>();

    // get local/global rotation of each joint
    Quat parent_localR = parent->local_rot();
    Quat parent_globalR = parent->world_rot();

    Quat child_localR = child->local_rot();
    Quat child_globalR = child->world_rot();

    {
        //** STEP1 **//
        // extend/contract the joint chain  
        // such that length of the vector from parent_worldPos to ee_worldPos matches 
        // the length of the vector from parent_worldPos to target_worldPos

        // clamp function
        auto clamp = [](float length, float min, float max)
        {
            if(length > max){
                return max;
            }
            else if(length < min){
                return min;
            }
            return length;
        };

        // get length
        float length_parentTochild = (child_worldPos - parent_worldPos).norm();
        float length_childToee = (child_worldPos-ee_worldPos).norm();
        float length_parentTotarget = clamp( (target_worldPos - parent_worldPos).norm(), epsilon, length_parentTochild + length_childToee - epsilon );

        // get current angles of joints
        Vec3 dir_parentToee = (ee_worldPos - parent_worldPos).normalized();
        Vec3 dir_parentTochild = (child_worldPos - parent_worldPos).normalized();
        float parent_currentAngle = a::gl::safe_acos( dir_parentToee.dot(dir_parentTochild) );

        Vec3 dir_childToparent = (parent_worldPos - child_worldPos).normalized();
        Vec3 dir_childToee = (ee_worldPos - child_worldPos).normalized();
        float child_currentAngle = a::gl::safe_acos( dir_childToparent.dot(dir_childToee) );

        // get desired angles of joints using 코사인 제2법칙
        float length_a = length_childToee * length_childToee;
        
        float parent_desiredAngle = a::gl::safe_acos( (length_parentTochild * length_parentTochild + length_parentTotarget * length_parentTotarget
                                                    - length_childToee * length_childToee)
                                                    /(2 * length_parentTochild * length_parentTotarget) );

        float child_desiredAngle = a::gl::safe_acos( (length_parentTochild * length_parentTochild + length_childToee * length_childToee 
                                                    - length_parentTotarget * length_parentTotarget )
                                                        /(2 * length_parentTochild * length_childToee) );
        
        // check whether the arm is fully stretched/folded or not
        bool isFullyStreched = false;
        if( std::fabs(parent_currentAngle) < epsilon || std::fabs(child_currentAngle) < epsilon )
        {
            isFullyStreched = true;
        }

        // get d: Z axis of child_globalR (used when the arm is fully stretched)
        Vec3 Zaxis = child_globalR * Vec3::UnitZ();

        // gaxis_parent: rotation axis of first rot
        Vec3 gaxis_parent = (isFullyStreched) ? 
            ((ee_worldPos - parent_worldPos).cross(Zaxis)).normalized() : 
            ((ee_worldPos - parent_worldPos).cross(child_worldPos - parent_worldPos)).normalized();    

        // axis 반대로. 
        if(isFullyStreched && use_opposite_dir)
        {
            gaxis_parent = -1.0f * gaxis_parent;
            //std::cout << "inside" << std::endl;
        }
        // change global axis to local axis
        Vec3 laxis_parent = (parent_globalR.inverse() * gaxis_parent).normalized();
        Vec3 laxis_child = (child_globalR.inverse() * gaxis_parent).normalized();
        
        // calculate how much angle do joints have to rotate
        Quat rot1_parent = Quat(AAxis(parent_desiredAngle - parent_currentAngle, laxis_parent));
        Quat rot1_child = Quat(AAxis(child_desiredAngle - child_currentAngle, laxis_child));

        // rotate the joints properly 어깨 joint -> 팔등 joint 
        //(these quat vectors should be all normalized since these can be scaled by multiplying each quat)
        Quat local_rot1_parent = (parent_localR * rot1_parent).normalized();
        Quat local_rot1_child = (child_localR * rot1_child).normalized();

        parent->set_local_rot(local_rot1_parent);
        child->set_local_rot(local_rot1_child);
    }

    {
        //** STEP2  Rotate the parent joint to target_worldPos **//

        //setting variables which have been updated
        parent -> update_world_trf_children();
        parent_worldPos = parent->world_pos();
        child_worldPos = child->world_pos();
        ee_worldPos = end_effector->world_pos();
        parent_localR = parent->local_rot();
        parent_globalR = parent->world_rot();

        // get rotation axis of parent
        Vec3 gaxis_parent = ((ee_worldPos - parent_worldPos).cross(target_worldPos - parent_worldPos)).normalized();
        Vec3 laxis_parent = (parent_globalR.inverse() * gaxis_parent).normalized();

        // calculate how much rotation do joints have to rotate
        Vec3 dir_parentToee = (ee_worldPos - parent_worldPos).normalized();
        Vec3 dir_parentTotarget = (target_worldPos - parent_worldPos).normalized();
        float parent_desiredAngle = a::gl::safe_acos( dir_parentToee.dot(dir_parentTotarget));
        Quat rot2_parent = Quat(AAxis(parent_desiredAngle, laxis_parent));
        
        // rotate the parent joint
        Quat local_rot2_parent = (parent_localR * rot2_parent).normalized();

        parent->set_local_rot(local_rot2_parent);
    }
}

}
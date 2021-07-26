#pragma once
#include "../eigentype.h"
#include <memory>

namespace a::gl {

struct KinDisp;
struct KinModel;
using spKinModel = std::shared_ptr<KinModel>;

/**
 * @brief Kinematic pose vector
 */
struct KinPose
{
    // constructors and operators
    KinPose()                           = default;
    KinPose(const KinPose&)             = default;
    KinPose(KinPose&&)                  = default;

    KinPose& operator=(const KinPose&)  = default;
    KinPose& operator=(KinPose&&)       = default;

    // functions
    void add(const KinDisp& disp, const spKinModel& kmodel);
    void init_world_baseTrf_from_shoulders(int Ridx, int Lidx); // initialize baseTrf using shoulder joints
    void set_world_baseTrf(const Mat4& tar_baseTrf);
    void set_world_baseTrf(const Mat4& cur_baseTrf, const Mat4& tar_baseTrf);

    // ! 이 정보들은 바로 수정하지 말것!
    Mat4                world_baseTrf;
    std::vector<Mat4>   world_trfs;
    
    Vec3                local_T0;
    std::vector<Mat4>   local_Rs;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/**
 * @brief Kinematic displacement vector.
 */
struct KinDisp
{
    // constructors and operators
    KinDisp()                           = default;
    KinDisp(const KinDisp&)             = default;
    KinDisp(KinDisp&&)                  = default;
    
    /**
     * @brief           Create Displacement
     *                  dp = p1 - p0
     * 
     * @param p0        current pose
     * @param p1        next pose
     * @return          displacement (dp). baseTrf은 p0와 같음.
     */
    KinDisp(const KinPose& p0, const KinPose& p1);
    
    KinDisp& operator=(const KinDisp&)  = default;
    KinDisp& operator=(KinDisp&&)       = default;

    void set_world_baseTrf(const Mat4& tar_baseTrf);

    // variables
    Mat4                world_baseTrf;
    Vec3                local_T0;
    std::vector<Mat4>   local_Rs;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

namespace kin {

/**
 * @brief           Create Displacement
 *                  dp = p1 - p0
 * 
 * @param p0        current pose
 * @param p1        next pose
 * @return          displacement (dp). baseTrf은 p0와 같음.
 */
KinDisp     displacement(const KinPose& p0, const KinPose& p1);
void        add_displacement(KinPose& self, const KinDisp& disp, const spKinModel& kmodel, float w = 1.0f);

void        init_world_baseTrf_from_shoulders(KinPose& self, int Ridx, int Lidx); // initialize baseTrf using shoulder joints
void        set_world_baseTrf(KinPose& self, const Mat4& tar_baseTrf);
void        set_world_baseTrf(KinDisp& self, const Mat4& tar_baseTrf);
void        set_world_baseTrf(KinPose& self, const Mat4& cur_baseTrf, const Mat4& tar_baseTrf);

}
}
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
    void init_world_basisTrf_from_shoulders(int Ridx, int Lidx); // initialize baseTrf using shoulder joints
    void set_world_basisTrf(const Mat4& tar_basis);
    void set_world_basisTrf(const Mat4& cur_basis, const Mat4& tar_basis);

    // ! 이 정보들은 바로 수정하지 말것!
    Mat4                world_basisTrf;
    std::vector<Mat4>   world_trfs;
    
    // ! root의 경우는 world 와 같음.
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

    void set_world_basisTrf(const Mat4& tar_basis);

    // variables
    Mat4                world_basisTrf;
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

void        init_world_basisTrf_from_shoulders(KinPose& self, int Ridx, int Lidx); // initialize basis using shoulder joints
void        set_world_basisTrf(KinPose& self, const Mat4& tar_basis);
void        set_world_basisTrf(KinDisp& self, const Mat4& tar_basis);
void        set_world_basisTrf(KinPose& self, const Mat4& cur_basis, const Mat4& tar_basis);

}
}
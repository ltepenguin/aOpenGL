#pragma once
#include "../eigentype.h"
#include <memory>

namespace a::gl {

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
    void init_world_basisTrf_from_shoulders(int Ridx, int Lidx); // initialize baseTrf using shoulder joints
    void move_world_basisTrf(const Mat4& tar_basis);
    void move_world_basisTrf(const Mat4& cur_basis, const Mat4& tar_basis);
    void recompute_local_root();

    // ! 이 정보들은 바로 수정하지 말것!
    Mat4                world_basisTrf;
    std::vector<Mat4>   world_trfs;
    
    // ! root_preR 은 fixed. 편의성을 위해서..
    Mat4                root_preR;

    // ! root의 경우 basis에 상대적인 transform.
    // * rootTrf = basisTrf * (local_pos * preR) * rootLocalRot
    // * jntTrf = parentTrf * preTrf * localTrf
    Vec4                local_pos;
    std::vector<Mat4>   local_rots; // root 이외의 나머지 joint rotation 값들은 보통 fixed.

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

namespace kin {

void        recompute_local_root(KinPose& self);

/**
 * @brief This function changes the basis and local root transformation.
 */
void        init_world_basisTrf_from_shoulders(KinPose& self, int Ridx, int Lidx); // initialize basis using shoulder joints

/**
 * @brief This function does not change the world transformation.
 */
void        reset_world_basisTrf(KinPose& self, const Mat4& basisTrf);

/**
 * @brief This function changes the world transformation.
 */
void        move_world_basisTrf(KinPose& self, const Mat4& tar_basis);

/**
 * @brief This function changes the world transformation.
 *        Local root info. is recalculated.
 */
void        move_world_basisTrf(KinPose& self, const Mat4& cur_basis, const Mat4& tar_basis);

}
}
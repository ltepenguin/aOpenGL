#pragma once
#include "../eigentype.h"
#include "kindisp.h"
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

    /**
     * @return projected root trf object
     */
    Mat4 get_projected_root_trf() const;

    void init_world_basisTrf();

    void init_world_basisTrf_from_shoulders(int Ridx, int Lidx); // initialize baseTrf using shoulder joints
    
    /**
     * @brief This function changes the world transformation.
     */    
    void move_world_basisTrf(const Mat4& tar_basis);
    
    /**
     * @brief This function changes the world transformation.
     *        ! 주의: tar_trf 으로 basis를 옮기는 것이 아님.
     */    
    void move_world_basisTrf_anchor(const Mat4 src_trf, const Mat4& tar_trf);
    
    /**
     * @brief This function does not change the world transformation.
     */
    void reset_world_basisTrf(const Mat4& basisTrf);

    /**
     * @brief recompute local_pos and local_rots.at(0)
     */
    void recompute_local_root();

    /**
     * @brief see KinDisp for detailed implementation
     */
    void add(const KinDisp& disp, spKinModel kmodel);
    void add(const KinDisp& disp, spKinModel kmodel, float weight);

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

void recompute_local_root(KinPose& self);

Mat4 get_projected_root_trf(const KinPose& self);

void init_world_basisTrf(KinPose& self);

/**
 * @brief This function changes the basis and local root transformation.
 */
void init_world_basisTrf_from_shoulders(KinPose& self, int Ridx, int Lidx); // initialize basis using shoulder joints

/**
 * @brief This function does not change the world transformation.
 */
void reset_world_basisTrf(KinPose& self, const Mat4& basisTrf);

/**
 * @brief This function changes the world transformation.
 */
void move_world_basisTrf(KinPose& self, const Mat4& tar_basis);

/**
 * @brief This function changes the world transformation.
 */
void move_world_basisTrf_anchor(KinPose& self, const Mat4& cur_trf, const Mat4& tar_trf);

/**
 * @brief add displacement
 */
void add(KinPose& pose, const KinDisp& disp, spKinModel kmodel);
void add(KinPose& pose, const KinDisp& disp, spKinModel kmodel, float weight);

}
}
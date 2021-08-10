#pragma once
#include "kinpose.h"

namespace a::gl {

struct KinPose;

/**
 * @brief
 */
struct KinDisp
{
    // constructors
    KinDisp()                           = default;
    KinDisp(const KinDisp&)             = default;
    KinDisp(const KinPose& src, const KinPose& tar);

    KinDisp& operator=(const KinDisp&)  = default;
    KinDisp& operator=(KinDisp&&)       = default;

    // functions
    void move_world_basisTrf(const Mat4& tar_basis);
    void scale(float weight);

    // displacements
    Mat4 world_basisTrf;
    Mat4 d_world_basisTrf;
    
    // locals
    Vec4 d_local_pos;
    std::vector<Mat4> d_local_rots;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

namespace kin {

void move_world_basisTrf(KinDisp& self, const Mat4& tar_basis);

void scale(KinDisp& disp, float weight);

}

}
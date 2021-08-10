#include "aOpenGL/kin/kindisp.h"
#include "aOpenGL/kin/kinmodel.h"

namespace a::gl {

KinDisp::KinDisp(const KinPose& src, const KinPose& tar)
{
    this->world_basisTrf = src.world_basisTrf;
    
    this->d_world_basisTrf = tar.world_basisTrf * src.world_basisTrf.inverse();
    this->d_local_pos = tar.local_pos - src.local_pos;
    this->d_local_pos.w() = 0.0f;
    
    int noj = src.local_rots.size();
    this->d_local_rots.resize(noj, Mat4::Identity());
    for(int i = 0; i < noj; ++i)
    {
        this->d_local_rots.at(i) = tar.local_rots.at(i) * src.local_rots.at(i).inverse();
    }
}

void KinDisp::move_world_basisTrf(const Mat4& tar_basis)
{
    return kin::move_world_basisTrf(*this, tar_basis);
}

void KinDisp::scale(float weight)
{
    return kin::scale(*this, weight);
}

namespace kin {

void move_world_basisTrf(KinDisp& self, const Mat4& tar_basis)
{
    // basisA -> basisB
    //   dTrf * basisA = basisB
    //   dTrf = basisB * basisA.inv
    //
    // T * basisA -> T * basisB
    //   dTrf' * (T * basisA) = T * basisB
    //   dTrf' = T * basisB * (T * basisA).inv
    //         = T * basisB * basisA.inv * T.inv
    //         = T * dTrf * T.inv

    Mat4 T = tar_basis * self.world_basisTrf.inverse();
    self.d_world_basisTrf = T * self.d_world_basisTrf * T.inverse();
    self.world_basisTrf = tar_basis;
}

//#define use_quaternion_scale

void scale(KinDisp& disp, float weight)
{
    // basis dPos
    {
        disp.d_world_basisTrf.col(3).head<3>() = weight * disp.d_world_basisTrf.col(3).head<3>();
    }
    
    // basis dRot
    {
        Mat3 R = disp.d_world_basisTrf.block<3, 3>(0, 0);
#ifdef use_quaternion_scale
        Quat q(R);
        R = q.slerp(1.0f - weight, Quat::Identity()).toRotationMatrix();
#else
        AAxis aaxis(R);
        aaxis.angle() = weight * aaxis.angle();
        R = aaxis.toRotationMatrix();
#endif
        disp.d_world_basisTrf.block<3, 3>(0, 0) = R;
    }
    
    // local pos
    {
        disp.d_local_pos = weight * disp.d_local_pos;
    }

    // local rot
    int noj = disp.d_local_rots.size();
    for(int i = 0; i < noj; ++i)
    {
        Mat3 Ri = disp.d_local_rots.at(i).block<3, 3>(0, 0);
#ifdef use_quaternion_scale
        Quat qi(Ri);
        Ri = qi.slerp(1.0f - weight, Quat::Identity()).toRotationMatrix();
#else
        AAxis aaxis(Ri);
        aaxis.angle() = weight * aaxis.angle();
        Ri = aaxis.toRotationMatrix();
#endif
        disp.d_local_rots.at(i).block<3, 3>(0, 0) = Ri;
    }

}

}

}
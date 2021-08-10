#include "aOpenGL/kin/kinpose.h"
#include "aOpenGL/kin/kinmodel.h"
#include <iostream>

namespace a::gl {

void KinPose::init_world_basisTrf_from_shoulders(int Ridx, int Lidx)
{
    kin::init_world_basisTrf_from_shoulders(*this, Ridx, Lidx);
}

void KinPose::move_world_basisTrf(const Mat4& tar_basisTrf)
{
    kin::move_world_basisTrf(*this, tar_basisTrf);
}

void KinPose::move_world_basisTrf(const Mat4& cur_basisTrf, const Mat4& tar_basisTrf)
{
    kin::move_world_basisTrf(*this, cur_basisTrf, tar_basisTrf);
}

void KinPose::recompute_local_root()
{
    kin::recompute_local_root(*this);
}

namespace kin {

void recompute_local_root(KinPose& self)
{
    Mat4 basisInv = self.world_basisTrf.inverse();

    // root_pos = basisTrf * local_pos
    // local_pos = basisTrf.inv * root_pos
    self.local_pos = basisInv * self.world_trfs.at(0).col(3);

    // root_rot = basisTrf * preR * root_local_R
    // root_local_R = preR.inv * basisTrf.inv * root_rot
    Mat4 local_R = self.root_preR.inverse() * basisInv * self.world_trfs.at(0);
    self.local_rots.at(0).block<3, 3>(0,0) = local_R.block<3, 3>(0, 0);
}

void init_world_basisTrf_from_shoulders(KinPose& self, int Ridx, int Lidx)
{
    Vec3 root_x = self.world_trfs.at(0).col(0).head<3>();
    root_x.y() = 0;
    root_x.normalize();

    Vec3 Rp = self.world_trfs.at(Ridx).col(3).head<3>();
    Vec3 Lp = self.world_trfs.at(Lidx).col(3).head<3>();

    Vec3 shl_x = Lp - Rp;
    shl_x.y() = 0;
    shl_x.normalize();    
    
    // average
    Vec3 basisX = (shl_x + root_x).normalized();
    Vec3 basisZ = basisX.cross(Vec3::UnitY());
    
    // set orientation
    self.world_basisTrf.block<3, 1>(0, 0) = basisX;
    self.world_basisTrf.block<3, 1>(0, 1) = Vec3::UnitY();
    self.world_basisTrf.block<3, 1>(0, 2) = basisZ;

    // set position
    Vec3 shoulder_pos = 0.5f * (Rp + Lp).block<3, 1>(0, 0);
    Vec3 basis_pos = self.world_basisTrf.col(3).head<3>();
    Vec3 T  = 0.5f * (basis_pos + shoulder_pos);
    T.y() = 0.0f;
    self.world_basisTrf.block<3, 1>(0, 3) = T;

    // set local root
    kin::recompute_local_root(self);
}

void reset_world_basisTrf(KinPose& self, const Mat4& basisTrf)
{
    self.world_basisTrf = basisTrf;
    kin::recompute_local_root(self);
}

void move_world_basisTrf(KinPose& self, const Mat4& tar_basisTrf)
{
    move_world_basisTrf(self, self.world_basisTrf, tar_basisTrf);
}

void move_world_basisTrf(KinPose& self, const Mat4& cur_basisTrf, const Mat4& tar_basisTrf)
{
    // set world transform
    Mat4 inv_btrf = cur_basisTrf.inverse();
    Mat4 d_trf = tar_basisTrf * inv_btrf;
    for(auto& wtrf : self.world_trfs)
    {
        wtrf = d_trf * wtrf;
    }
    self.world_basisTrf = tar_basisTrf;

    // set local root
    kin::recompute_local_root(self);
}

}}
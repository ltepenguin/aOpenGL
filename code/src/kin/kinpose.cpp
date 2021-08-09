#include "aOpenGL/kin/kinpose.h"
#include "aOpenGL/kin/kinmodel.h"
#include <iostream>

namespace a::gl {

void KinPose::add(const KinDisp& disp, const spKinModel& kmodel)
{
    kin::add_displacement(*this, disp, kmodel);
}

void KinPose::init_world_basisTrf_from_shoulders(int Ridx, int Lidx)
{
    kin::init_world_basisTrf_from_shoulders(*this, Ridx, Lidx);
}

void KinPose::set_world_basisTrf(const Mat4& tar_basisTrf)
{
    kin::set_world_basisTrf(*this, tar_basisTrf);
}

void KinPose::set_world_basisTrf(const Mat4& cur_basisTrf, const Mat4& tar_basisTrf)
{
    kin::set_world_basisTrf(*this, cur_basisTrf, tar_basisTrf);
}

KinDisp::KinDisp(const KinPose& p0, const KinPose& p1)
{
    KinPose p1copy = p1;
    kin::set_world_basisTrf(p1copy, p1copy.world_basisTrf, p0.world_basisTrf);

    int noj = p0.local_Rs.size();

    // set basisTrf
    {
        this->world_basisTrf = p0.world_basisTrf;
    }
    
    // set translation
    {
        this->local_T0 = p1copy.local_T0 - p0.local_T0;
    }
    
    // set rotation
    {
        this->local_Rs.reserve(noj);
        for(int i = 0; i < noj; ++i)
        {
            Mat4 dR = p1copy.local_Rs.at(i) * p0.local_Rs.at(i).transpose();
            this->local_Rs.push_back(dR);
        }
    }
}

void KinDisp::set_world_basisTrf(const Mat4& tar_basisTrf)
{
    kin::set_world_basisTrf(*this, tar_basisTrf);
}

namespace kin {

KinDisp displacement(const KinPose& p0, const KinPose& p1)
{
    return KinDisp(p0, p1);
}

void add_displacement(KinPose& self, const KinDisp& disp, const spKinModel& kmodel, float w)
{
    assert(w >= 0.0f);

    KinDisp disp_copy = disp;
    kin::set_world_basisTrf(disp_copy, self.world_basisTrf);
    
    int noj = self.local_Rs.size();

    // local_T0
    {
        self.local_T0 += w * disp_copy.local_T0;
    }

    // local_Rs
    if(w >= 1.0f)
    {
        for(int i = 0; i < noj; ++i)
        {
            self.local_Rs.at(i) = disp_copy.local_Rs.at(i) * self.local_Rs.at(i);
        }
    }
    else
    {
        for(int i = 0; i < noj; ++i)
        {
            Quat qi(disp_copy.local_Rs.at(i).block<3, 3>(0, 0));
            
            // slerp
            Quat dq = qi.slerp(1.0f - w, Quat::Identity());
            dq.normalize();
            self.local_Rs.at(i) = to_mat4(dq) * self.local_Rs.at(i);
        }
    }

    // world transforms
    {
        self.world_trfs = kmodel->compute_fk(self.local_T0, self.local_Rs);
    }
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
}

void set_world_basisTrf(KinPose& self, const Mat4& tar_basisTrf)
{
    set_world_basisTrf(self, self.world_basisTrf, tar_basisTrf);
}

void set_world_basisTrf(KinDisp& self, const Mat4& tar_basisTrf)
{
    Mat3 curR = self.world_basisTrf.block<3, 3>(0, 0);
    Mat3 tarR = tar_basisTrf.block<3, 3>(0, 0);
    Mat3 dR = tarR * curR.inverse();
    self.local_T0 = dR * self.local_T0;

    self.local_Rs.at(0).block<3, 3>(0, 0) = dR * self.local_Rs.at(0).block<3, 3>(0, 0) * dR.inverse();
    self.world_basisTrf = tar_basisTrf;
}

void set_world_basisTrf(KinPose& self, const Mat4& cur_basisTrf, const Mat4& tar_basisTrf)
{
    Mat4 inv_btrf = cur_basisTrf.inverse();
    Mat4 d_trf = tar_basisTrf * inv_btrf;
    for(auto& wtrf : self.world_trfs)
    {
        wtrf = d_trf * wtrf;
    }

    Mat4 R0 = d_trf * self.local_Rs.at(0);
    Vec4 rootT = d_trf * Vec4(self.local_T0.x(), self.local_T0.y(), self.local_T0.z(), 1.0f);
    
    self.local_Rs.at(0).block<3, 3>(0, 0) = R0.block<3, 3>(0, 0);
    self.local_T0 = rootT.head<3>();
    self.world_basisTrf = tar_basisTrf;
}

}}
#include "aOpenGL/kin/kinmodel.h"
#include "aOpenGL/joint.h"
#include <iostream>

namespace a::gl {

/**
 * @brief fk order를 recursive하게 계산.
 */
static void _set_fk_order(int                     idx, 
                          const std::vector<int>& parent_idxes, 
                          std::vector<bool>&      updated, 
                          std::vector<int>&       order)
{
    if(idx >= (int)parent_idxes.size())
        return;
    
    assert(updated.at(idx) == false);

    int parent_idx = parent_idxes.at(idx);
    if(parent_idxes.at(idx) < 0)
    {        
        updated.at(idx) = true;
        order.push_back(idx);
        return _set_fk_order(idx + 1, parent_idxes, updated, order);
    }
    else
    {
        if(updated.at(parent_idx) == false)
        {
            _set_fk_order(parent_idx, parent_idxes, updated, order);
        }
        updated.at(idx) = true;
        order.push_back(idx);
        return _set_fk_order(idx + 1, parent_idxes, updated, order);
    } 
}

spKinModel kinmodel(const spModel& model)
{
    const auto& joints = model->joints();
    std::vector<std::string> jnt_names;
    jnt_names.reserve(joints.size());
    
    for(int i = 0; i < (int)joints.size(); ++i)
        jnt_names.push_back(joints.at(i)->name());
    
    return kinmodel(model, jnt_names);
}

spKinModel kinmodel(const spModel& model, const std::vector<std::string>& jnt_names)
{
    const auto joints = model->joints(jnt_names);
    int noj = joints.size();
    
    std::vector<Mat4>          pre_trfs;
    std::map<std::string, int> jnt_name_to_idx;
    std::vector<int>           parent_idxes;
    std::vector<int>           gl_jnt_idxes;
    pre_trfs.reserve(noj);
    parent_idxes.reserve(noj);
    gl_jnt_idxes.reserve(noj);

    for(int i = 0; i < noj; ++i)
    {
        auto& jnt = joints.at(i);
        pre_trfs.push_back(jnt->pre_trf());
        jnt_name_to_idx[jnt->name()] = i;
        gl_jnt_idxes.push_back(model->joint_idx(jnt->name()));
    }

    for(int i = 0; i < noj; ++i)
    {
        auto& jnt = joints.at(i);
        if(jnt->parent() != nullptr)
        {
            auto find_jnt_idx = jnt_name_to_idx.find(jnt->parent()->name());
            if(find_jnt_idx == jnt_name_to_idx.end())
            {
                std::cerr << "joint " << jnt->parent()->name() << " is not found." << std::endl;
                assert(false);
            }

            int parent_idx = jnt_name_to_idx.at(jnt->parent()->name());
            parent_idxes.push_back(parent_idx);
        }
        else
        {
            assert(i == 0); // root index must be zero
            parent_idxes.push_back(-1);
        }
    }

    // fk order
    std::vector<int>  fk_order;
    std::vector<bool> updated(noj, false);
    _set_fk_order(0, parent_idxes, updated, fk_order);

    // create kinmodel
    spKinModel kmodel = std::make_shared<KinModel>();
    kmodel->noj             = noj;
    kmodel->jnt_names       = jnt_names;
    kmodel->pre_trfs        = pre_trfs;
    kmodel->parent_idxes    = parent_idxes;
    kmodel->fk_order        = fk_order;
    kmodel->jnt_name_to_idx = jnt_name_to_idx;
    kmodel->gl_jnt_idxes    = gl_jnt_idxes;
    return kmodel;
}

std::vector<Mat4> KinModel::compute_fk(const Mat4& basisTrf, const Vec4& local_pos, const std::vector<Quat>& local_rots)
{
    return kin::compute_fk(shared_from_this(), basisTrf, local_pos, local_rots);
}

std::vector<Mat4> KinModel::compute_fk(const Mat4& basisTrf, const Vec4& local_pos, const std::vector<Mat4>& local_rots)
{
    return kin::compute_fk(shared_from_this(), basisTrf, local_pos, local_rots);
}

// Functions ------------------------------------------------------- //

namespace kin {

std::vector<Mat4> compute_fk(const spKinModel&        kmodel,
                             const Mat4&              basisTrf,
                             const Vec4&              local_pos,
                             const std::vector<Quat>& local_rots)
{
    std::vector<Mat4> local_Rs_trfs = a::gl::to_mat4<Quat>(local_rots);
    return compute_fk(kmodel, basisTrf, local_pos, local_Rs_trfs);
}

std::vector<Mat4> compute_fk(const spKinModel&        kmodel,
                             const Mat4&              basisTrf,
                             const Vec4&              local_pos,
                             const std::vector<Mat4>& local_rots)
{
    std::vector<Mat4> world_trfs;
    world_trfs.resize(kmodel->noj, Mat4::Identity());
    
    const auto& fk_order     = kmodel->fk_order;
    const auto& parent_idxes = kmodel->parent_idxes;
    for(int i = 0; i < (int)fk_order.size(); ++i)
    {
        int jnt_idx = fk_order.at(i);
        int par_idx = parent_idxes.at(jnt_idx);

        if(par_idx < 0) // root joint (no parent)
        {
            Mat4 pre_trf = kmodel->pre_trfs.at(jnt_idx);
            pre_trf.col(3) = local_pos;
            world_trfs.at(jnt_idx) = basisTrf * pre_trf * local_rots.at(jnt_idx);
        }
        else
        {
            world_trfs.at(jnt_idx)
                = world_trfs.at(par_idx) * kmodel->pre_trfs.at(jnt_idx) * local_rots.at(jnt_idx);
        }
    }
    return world_trfs;
}

}
}
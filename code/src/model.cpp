#include "aOpenGL/model.h"
#include "aOpenGL/joint.h"
#include "aOpenGL/mesh.h"

namespace a::gl {

Model::Model(std::vector<spJoint>& joints):
    m_joints(joints), 
    m_jnt_name_to_idx(a::gl::joints_name_to_idx_map(joints)), 
    m_meshes()
{}

Model::Model(const std::vector<std::pair<core::spMeshGL, vMaterial>>& gl_meshes):
    m_joints(),
    m_jnt_name_to_idx(),
    m_meshes()
{
    // create joints
    auto root_obj = std::make_shared<Joint>();
    root_obj->set_name("root");
    m_joints.push_back(root_obj);
    m_jnt_name_to_idx = a::gl::joints_name_to_idx_map(m_joints);
    
    //set meshes
    int mesh_n = gl_meshes.size();
    m_meshes.reserve(mesh_n);
    for(int i = 0; i < mesh_n; ++i)
    {
        auto mesh = std::make_shared<Mesh>(gl_meshes.at(i).first);
        mesh->set_materials(gl_meshes.at(i).second);
        m_meshes.push_back(mesh);
    }
}

Model::Model(std::vector<spJoint>& joints, 
             const std::vector<std::pair<core::spMeshGL, vMaterial>>& gl_meshes):
    m_joints(joints),
    m_jnt_name_to_idx(a::gl::joints_name_to_idx_map(joints)),
    m_meshes()
{
    assert(joints.size() > 0);

    // set meshes
    int mesh_n = gl_meshes.size();
    m_meshes.reserve(mesh_n);
    for(int i = 0; i < mesh_n; ++i)
    {
        auto mesh = std::make_shared<Mesh>(joints, gl_meshes.at(i).first);
        mesh->set_materials(gl_meshes.at(i).second);
        m_meshes.push_back(mesh);
    }
}

Model::Model(std::vector<spJoint>& joints, std::vector<spMesh>& meshes):
    m_joints(joints),
    m_jnt_name_to_idx(a::gl::joints_name_to_idx_map(joints)), 
    m_meshes(meshes)
{}

spModel Model::copy() const
{
    auto copied = a::gl::joints_copy(m_joints);
    int mesh_n = m_meshes.size();

    if(mesh_n == 0)
    {
        return std::make_shared<Model>(copied);
    }
    else
    {
        std::vector<spMesh> meshes;
        meshes.reserve(mesh_n);
        for(int i = 0; i < mesh_n; ++i)
        {
            bool use_skinning = m_meshes.at(i)->use_skinning();
            auto mesh = use_skinning ? 
                std::make_shared<a::gl::Mesh>(copied, *m_meshes.at(i).get()) :
                std::make_shared<a::gl::Mesh>(*m_meshes.at(i).get());
            
            mesh->set_materials(m_meshes.at(i)->materials());
            meshes.push_back(mesh);
        }
        return std::make_shared<Model>(copied, meshes);
    }
}

void Model::update_mesh()
{
    for(auto& mesh : m_meshes)
        mesh->update_mesh();
}

int Model::mesh_number() const 
{ 
    return m_meshes.size();
}

spMesh Model::mesh(int mesh_id)
{
    return m_meshes.at(mesh_id); 
}

const spMesh Model::mesh(int mesh_id) const
{
    return m_meshes.at(mesh_id);
}

std::vector<spMesh> Model::meshes()
{
    return m_meshes;
}

const std::vector<spMesh> Model::meshes() const
{
    return m_meshes;
}

spJoint Model::root()
{
    return m_joints.at(0);
}

const spJoint Model::root() const
{
    return m_joints.at(0);
}

std::vector<spJoint> Model::joints()
{
    return m_joints;
}

const std::vector<spJoint> Model::joints() const
{
    return m_joints;
}

spJoint Model::joint(const std::string& name)
{
    auto iter = m_jnt_name_to_idx.find(name);
    if(iter != m_jnt_name_to_idx.end())
        return m_joints.at(iter->second);
    else
        return nullptr;
}

const spJoint Model::joint(const std::string& name) const
{
    auto iter = m_jnt_name_to_idx.find(name);
    if(iter != m_jnt_name_to_idx.end())
        return m_joints.at(iter->second);
    else
        return nullptr;
}

spJoint Model::joint(int jidx)
{
    return joints().at(jidx);
}

const spJoint Model::joint(int jidx) const
{
    return joints().at(jidx);
}

std::vector<spJoint> Model::joints(const std::vector<std::string>& names)
{
    std::vector<spJoint> results;
    results.reserve(names.size());
    
    for(int i = 0; i < names.size(); ++i)
    {
        results.push_back(this->joint(names.at(i)));
    }

    return results;
}

const std::vector<spJoint> Model::joints(const std::vector<std::string>& names) const
{
    std::vector<spJoint> results;
    results.reserve(names.size());
    
    for(int i = 0; i < names.size(); ++i)
    {
        results.push_back(this->joint(names.at(i)));
    }

    return results;
}

int Model::joint_idx(const std::string& name) const
{
    return m_jnt_name_to_idx.at(name);
}

void Model::set_pose(const Pose& pose)
{
    assert(pose.local_rotations.size() == m_joints.size());
    this->root()->set_local_pos(pose.root_position);
    
    for(int i = 0; i < m_joints.size(); ++i)
    {
        m_joints.at(i)->set_local_rot(pose.local_rotations.at(i));
    }
    
    this->root()->update_world_trf_children();
}

void Model::set_pose(const Pose& pose, const std::vector<std::string>& names)
{
    this->root()->set_local_pos(pose.root_position);

    for(int i = 0; i < (int)names.size(); ++i)
    {
        int idx = m_jnt_name_to_idx.at(names.at(i));
        m_joints.at(idx)->set_local_rot(pose.local_rotations.at(idx));
    }
    
    this->root()->update_world_trf_children();
}

void Model::set_pose(const KinPose& pose, const std::vector<std::string>& names)
{
    Vec3 root_pos = pose.world_trfs.at(0).col(3).head<3>();
    Mat3 root_rot = pose.world_trfs.at(0).block<3, 3>(0, 0);
    this->root()->set_local_pos(root_pos);
    this->root()->set_world_rot(root_rot);

    // other root
    for(int i = 1; i < (int)names.size(); ++i)
    {
        int idx = m_jnt_name_to_idx.at(names.at(i));
        m_joints.at(idx)->set_local_rot(pose.local_rots.at(i));
    }
    
    this->root()->update_world_trf_children();
}

void Model::set_current_pose_identity_rot()
{
    Mat3 I3  = Mat3::Identity();
    for(int i = 0; i < (int)m_joints.size(); ++i)
    {
        Mat3 new_pre_rot = m_joints.at(i)->pre_rot_mat() * m_joints.at(i)->local_rot_mat();
        m_joints.at(i)->set_pre_rot(new_pre_rot);
        m_joints.at(i)->set_local_rot(I3);
    }
}

}
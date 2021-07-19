#include "aOpenGL/mesh.h"
#include "aOpenGL/util.h"
#include "aOpenGL/joint.h"

namespace a::gl {

Mesh::Mesh(const a::gl::core::spMeshGL meshGL):
    m_meshGL(meshGL),
    m_materials(),
    m_use_skinning(false),
    m_joints(),
    m_jnt_name_to_idx(),
    m_buffer(),
    m_jnt_buffer_idx()
{}

Mesh::Mesh(std::vector<spJoint>& joints, const a::gl::core::spMeshGL meshGL):
    m_meshGL(meshGL),
    m_materials(),
    m_use_skinning(true),
    m_joints(joints),
    m_jnt_name_to_idx(a::gl::joints_name_to_idx_map(joints)),
    m_buffer(),
    m_jnt_buffer_idx()
{
    assert(meshGL->is_skinned);
    
    int noj = meshGL->joint_order.size();
    m_jnt_buffer_idx.reserve(noj);
    for(int j = 0; j < noj; ++j)
    {
        int jidx = m_jnt_name_to_idx[meshGL->joint_order.at(j)];
        m_jnt_buffer_idx.push_back(jidx);
    }
}

Mesh::Mesh(const Mesh& other):
    m_meshGL(other.m_meshGL),
    m_materials(),
    m_use_skinning(false),
    m_joints(),
    m_jnt_name_to_idx(),
    m_buffer(),
    m_jnt_buffer_idx()
{}

Mesh::Mesh(std::vector<spJoint>& joints, const Mesh& other):
    m_meshGL(other.m_meshGL),
    m_materials(),
    m_use_skinning(true),
    m_joints(joints),
    m_jnt_name_to_idx(other.m_jnt_name_to_idx),
    m_buffer(other.m_buffer),
    m_jnt_buffer_idx(other.m_jnt_buffer_idx)
{
    assert(other.m_meshGL->is_skinned);
    assert(joints.size() == other.m_joints.size());
}

void Mesh::set_materials(const std::vector<Material>& materials)
{
    m_materials = materials;
}

void Mesh::update_mesh()
{
    if(m_joints.size() == 0)
        return;
    
    m_joints.at(0)->update_world_trf_children();

    const auto& jnt_order = m_jnt_buffer_idx;
    
    m_buffer.clear();
    m_buffer.resize(jnt_order.size(), glm::mat4(1.0f));

    for(int i = 0; i < jnt_order.size(); ++i)
    {
        int jidx = jnt_order.at(i);
        glm::mat4 wtrf = a::gl::to_glm(m_joints.at(jidx)->world_trf());
        const glm::mat4& btrf_inv = m_meshGL->jonit_bind_trf_inv.at(i);
        m_buffer.at(i) = wtrf * btrf_inv;
    }
}

int Mesh::vertex_num() const
{
    return m_meshGL->vertices.size();
}

bool Mesh::use_skinning() const
{
    return m_use_skinning;
}

}
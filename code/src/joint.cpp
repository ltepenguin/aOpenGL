#include "aOpenGL/joint.h"
#include "aOpenGL/util.h"
#include <map>

namespace a::gl {

Joint::Joint():
    m_name(""),
    m_parent(nullptr),
    m_children(),
    m_world_trf(Mat4::Identity()),
    m_local_rot(Mat4::Identity()),
    m_pre_trf(Mat4::Identity()),
    m_scale(Mat4::Identity()),
    m_world_updated(false),
    m_local_skel(Mat4::Identity()),
    m_skel_length(0.0f)
{}

Joint::Joint(const Joint& jnt):
    m_name(jnt.m_name),
    m_parent(jnt.m_parent),
    m_children(),
    m_world_trf(jnt.m_world_trf),
    m_local_rot(jnt.m_local_rot),
    m_pre_trf(jnt.m_pre_trf),
    m_scale(jnt.m_scale),
    m_world_updated(jnt.m_world_updated),
    m_local_skel(jnt.m_local_skel),
    m_skel_length(jnt.m_skel_length)
{}

Joint::~Joint()
{}

std::string Joint::name() const
{
    return m_name;
}

void Joint::set_name(std::string name)
{
    m_name = name;
}

void Joint::set_world_trf_dirty()
{
	if(m_world_updated == false)
		return;
    
	m_world_updated = false;
	
    for(auto child : m_children)
		child->set_world_trf_dirty();
}

bool Joint::is_world_trf_dirty() const
{
    return !m_world_updated;
}

void Joint::set_pre_rot(const Quat& rotation)
{
    m_pre_trf.block<3, 3>(0, 0) = rotation.toRotationMatrix();
    this->set_world_trf_dirty();
}

void Joint::set_pre_rot(const Mat3& rotation)
{
    m_pre_trf.block<3, 3>(0, 0) = rotation;
    this->set_world_trf_dirty();
}

void Joint::set_local_pos(const Vec3& translation)
{
    m_pre_trf.block<3, 1>(0, 3) = translation;
    this->set_world_trf_dirty();
}

void Joint::set_local_pos(const Mat4& translation)
{
    m_pre_trf.col(3).head<3>() = translation.col(3).head<3>();
    this->set_world_trf_dirty();
}

void Joint::add_local_pos(const Vec3& translation)
{
    m_pre_trf.col(3).head<3>() += translation;
    this->set_world_trf_dirty();
}

void Joint::set_local_rot(const Quat& rotation)
{
    m_local_rot.block<3, 3>(0, 0) = rotation.toRotationMatrix();
    this->set_world_trf_dirty();
}

void Joint::set_local_rot(const Mat3& rotation)
{
    m_local_rot.block<3, 3>(0, 0) = rotation;
    this->set_world_trf_dirty();
}

void Joint::set_local_rot(const Mat4& rotation)
{
    m_local_rot.block<3, 3>(0, 0) = rotation.block<3, 3>(0, 0);
    this->set_world_trf_dirty();
}

void Joint::add_local_rot(const Quat& rotation)
{
    this->set_local_rot(rotation * this->local_rot());
}

void Joint::add_local_rot(const Mat3& rotation)
{
    Mat3 rot_mat = rotation * this->local_rot_mat();
    this->set_local_rot(rot_mat);
}

void Joint::set_scale(const Vec3& scale)
{
    m_scale(0, 0) = scale.x();
    m_scale(1, 1) = scale.y();
    m_scale(2, 2) = scale.z();
}

spJoint Joint::parent()
{
    return m_parent;
}

std::vector<spJoint> Joint::children()
{
    return m_children;
}

void Joint::set_parent(spJoint jnt)
{
    m_parent = jnt;

    // set skeleton
    {
        if(m_parent == nullptr)
            return;

        Vec3 dPos = this->world_pos() - m_parent->world_pos();
        float length = dPos.norm();

        Vec3 target_y_axis = dPos.normalized();
        Vec3 y_axis = Vec3::UnitY();
        Vec3 rot_axis = y_axis.cross(target_y_axis).normalized();

        float sangle = a::gl::get_sangle(y_axis, target_y_axis, rot_axis);
        Mat3 rq = AAxis(sangle, rot_axis).toRotationMatrix();
        
        Mat4 wtrf = a::gl::to_mat4(rq);
        Mat4 parent_wtrf = m_parent->world_trf();
        parent_wtrf.col(3) = Vec4(0, 0, 0, 1);

        m_local_skel = parent_wtrf.transpose() * wtrf;
        m_skel_length = length;
    }
}

void Joint::add_child(spJoint jnt)
{
    m_children.push_back(jnt);
}

void Joint::set_children(std::vector<spJoint>& jnts)
{
    m_children = jnts;
}

void Joint::set_world_rot(const Quat& rotation)
{
    this->set_world_rot(rotation.toRotationMatrix());
}

void Joint::set_world_rot(const Mat3& rotation)
{
    // wolrd_trf = parent_trf * pre_trf * R * S
    // R = pre_trf.inv * parent_trf.inv * world_trf * S.inv
    Mat3 preR = this->pre_rot_mat();
    Mat3 parentR = (m_parent == nullptr) ? Mat3::Identity() : m_parent->world_rot_mat();
    Mat3 localR = preR.transpose() * parentR.transpose() * this->world_rot_mat();
    this->set_local_rot(localR);
}

void Joint::add_world_rot(const Quat& rotation)
{
    auto rot = rotation * this->world_rot();
    this->set_world_rot(rot);
}

void Joint::add_world_rot(const Mat3& rotation)
{
    auto rot = rotation * this->world_rot_mat();
    this->set_world_rot(rot);
}

Quat Joint::local_rot() const
{
    return Quat(m_local_rot.block<3, 3>(0, 0));
}

Mat3 Joint::local_rot_mat() const
{
    return m_local_rot.block<3, 3>(0, 0);
}

Vec3 Joint::local_pos() const
{
    return m_pre_trf.col(3).head<3>();
}

Mat4 Joint::local_trf() const
{
    return m_pre_trf * m_local_rot * m_scale;
}

Mat4 Joint::local_rot_mat4() const
{
    return m_local_rot;
}

Mat4 Joint::local_pos_mat4() const
{
    Mat4 trf = Mat4::Identity();
    trf.col(3) = m_pre_trf.col(3);
    return trf;
}

Quat Joint::pre_rot() const
{
    return Quat(m_pre_trf.block<3, 3>(0, 0));
}

Mat3 Joint::pre_rot_mat() const
{
    return m_pre_trf.block<3, 3>(0, 0);
}

Mat4 Joint::pre_rot_mat4() const
{
    Mat4 trf = Mat4::Identity();
    trf.block<3, 3>(0, 0) = m_pre_trf.block<3, 3>(0, 0);
    return trf;
}

Mat4 Joint::pre_trf() const
{
    return m_pre_trf;
}

Quat Joint::world_rot()
{
    return Quat(this->world_rot_mat());
}

Mat3 Joint::world_rot_mat()
{
    return this->world_trf().block<3, 3>(0, 0);
}

Vec3 Joint::world_pos()
{
    return this->world_trf().col(3).head<3>();
}

Mat4 Joint::world_trf()
{
    this->update_world_trf();
    return m_world_trf;
}

Mat4 Joint::skel_world_trf()
{
    return m_parent->world_trf() * m_local_skel;
}

float Joint::skel_length() const
{
    return m_skel_length;
}

void Joint::update_world_trf()
{
    if(m_world_updated)
        return;
    
    if(m_parent != nullptr)
        m_world_trf = m_parent->world_trf() * this->local_trf();
    else
        m_world_trf = this->local_trf();
    
    m_world_updated = true;
}

void Joint::update_world_trf_children()
{
    this->update_world_trf();
    for(auto& child : m_children)
        child->update_world_trf_children();
}

std::vector<spJoint> joints_copy(const std::vector<spJoint>& jnts)
{
    int noj = jnts.size();
    std::vector<spJoint> copied;
    copied.reserve(noj);

    std::map<std::string, int> name_to_index;    
    for(int i = 0; i < noj; ++i)
    {
        auto clone_i = std::make_shared<Joint>(*jnts.at(i).get());
        copied.push_back(clone_i);
        name_to_index[clone_i->name()] = i;
    }

    for(int i = 0; i < noj; ++i)
    {
        const auto parent = jnts.at(i)->parent();
        if(parent != nullptr)
        {
            int pidx = name_to_index[parent->name()];
            
            auto child  = copied.at(i);
            auto parent = copied.at(pidx);
            child->set_parent(parent);
            parent->add_child(child);
        }
    }

    return copied;
}

std::vector<int> joints_parent_idxes(const std::vector<spJoint>& jnts)
{
    std::map<std::string, int> name2idx;
    for(int i = 0; i < jnts.size(); ++i)
        name2idx[jnts.at(i)->name()] = i;
    
    std::vector<int> indices;
    indices.reserve(jnts.size());
    for(int i = 0; i < jnts.size(); ++i)
    {
        if(jnts.at(i)->parent() == nullptr)
        {
            indices.push_back(-1);
        }
        else
        {
            int pidx = name2idx.at(jnts.at(i)->parent()->name());
            indices.push_back(pidx);
        }
    }
    return indices;
}

std::map<std::string, int> joints_name_to_idx_map(std::vector<spJoint>& joints)
{
    std::map<std::string, int> obj_map;
    for(int i = 0; i < joints.size(); ++i)
    {
        obj_map[joints.at(i)->name()] = i;
    }
    return obj_map;
}

}
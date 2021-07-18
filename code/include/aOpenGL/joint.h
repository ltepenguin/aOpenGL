#pragma once
#include "eigentype.h"
#include <memory>
#include <map>

namespace a::gl {

class Joint;
using spJoint = std::shared_ptr<Joint>;

/**
 * @brief Joint
 *
 * FBX computes the transformation matrix as follows:
 *      world_trf = parent_trf * T * Roff * Rp * Rpre * R * Rpost * RpInv * Soff * Sp * S * SpInv
 * Link: https://download.autodesk.com/us/fbx/20112/fbx_sdk_help/index.html?url=WS1a9193826455f5ff1f92379812724681e696651.htm,topicNumber=d0e7429
 * 
 * We use simplified version:
 *      wolrd_trf = parent_trf * (T * Rpre) * R * S
 * 
 * (T *  Rpre) is saved as m_pre_transform.
 * 
 */
class Joint
{
public:
    /**
     * @brief Construct a new Joint object
     */
    Joint();
    
    /**
     * @brief Construct a new Joint object. Children are not copied and the parent joint is shared.
     */
    Joint(const Joint&);
    
    /**
     * @brief Destroy the Joint object
     */
    ~Joint();
    
    std::string name() const;
    void set_name(std::string name);

    /**
     * @brief Set m_world_updated to false. m_world_trf needs re-computation.
     */
	void set_world_trf_dirty();

    /**
     * @return m_world_updated
     */
	bool is_world_trf_dirty() const;

    void set_pre_rot(const Quat& rotation);
	void set_pre_rot(const Mat3& rotation);	
    void set_local_pos(const Vec3& translation); // local_pos is translation of m_pre_trf
	void set_local_pos(const Mat4& translation); // local_pos is translation of m_pre_trf
    void add_local_pos(const Vec3& translation); // local_pos is translation of m_pre_trf
	void set_local_rot(const Quat& rotation);
	void set_local_rot(const Mat3& rotation);
	void set_local_rot(const Mat4& rotation);
	void add_local_rot(const Quat& rotation);
	void add_local_rot(const Mat3& rotation);
    void set_scale(const Vec3& scale);
	
    spJoint parent();
    std::vector<spJoint> children();

    /**
     * @brief set parent automatically computes m_local_skel
     */
    void set_parent(spJoint jnt);
    void add_child(spJoint jnt);
    void set_children(std::vector<spJoint>& jnts);

	void set_world_rot(const Quat& rotation);
	void set_world_rot(const Mat3& rotation);
	void add_world_rot(const Quat& rotation);
	void add_world_rot(const Mat3& rotation);

	Quat local_rot() const;
	Mat3 local_rot_mat() const;
	Vec3 local_pos() const;

    /**
     * @return m_pre_trf * m_local_rot * m_scale. 
     *         This is equal to: (T * Rpre) * R * S
     */
    Mat4 local_trf() const;

	Mat4 local_rot_mat4() const;
	Mat4 local_pos_mat4() const;
    
    // pre
    Quat pre_rot() const;
	Mat3 pre_rot_mat() const;
	Mat4 pre_rot_mat4() const;
    Mat4 pre_trf() const;

    // world
	Quat world_rot();
	Mat3 world_rot_mat();
	Vec3 world_pos();
	Mat4 world_trf();

	// skeleton
	Mat4  skel_world_trf();
	float skel_length() const;

    /**
     * @brief update m_world_trf and set m_world_updated to true
     */
	void update_world_trf();
	
    /**
     * @brief recursively update m_world_trf of cur and children. All m_world_updated are set to true.
     */
    void update_world_trf_children();

private:
    std::string          m_name;
    spJoint              m_parent;
    std::vector<spJoint> m_children;

    /**
     * @brief m_world_trf can be used if m_world_updated is true. Else, it needs recomputation.
     */
    Mat4                 m_world_trf;

    /**
     * @brief animation data
     */
    Mat4                 m_local_rot;

    /**
     * @brief In general, m_pre_trf is constant. (except for the translation of the root joint)
     */
    Mat4                 m_pre_trf;
    Mat4                 m_scale;
    
    /**
     * @brief if m_world_updated is false, m_world_trf must be recomputed.
     */
    bool                 m_world_updated;

    /**
     * @brief skel_world_trf = parent_trf * m_local_skel
     */
	Mat4                 m_local_skel;
	float                m_skel_length;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/**
 * @return deep-copied joints
 */
std::vector<spJoint> joints_copy(const std::vector<spJoint>& jnts);

/**
 * @return parent indices. 
 *         If there is no parent, set to -1. Size of vector is equal to size of jnts.
 */
std::vector<int> joints_parent_idxes(const std::vector<spJoint>& jnts);

/**
 * @return name -> index
 */
std::map<std::string, int> joints_name_to_idx_map(std::vector<spJoint>& jnts);

}
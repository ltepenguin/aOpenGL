#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include <memory>
#include "core/mesh.h"
#include "material.h"
#include "eigentype.h"

namespace a::gl {

class Joint;
class Mesh;
using spJoint = std::shared_ptr<Joint>;
using spMesh = std::shared_ptr<Mesh>;

/**
 * @brief Mesh 객체.
 * 
 * Skinning 된 경우, mesh buffer 관리.
 * 
 * world_vtx_pos = world_jnt_trf * local_vtx_pos
 *               = world_jnt_trf * jnt_bind_trf.inv * vtx_bind_pos
 *               = buffer_trf * vtx_bind_pos
 */
class Mesh
{
public:
    /**
     * @brief skinning 안된 mesh 객체 생성
     */
    explicit Mesh(const a::gl::core::spMeshGL meshGL);

    /**
     * @brief skinning 된 mesh 생성 할 경우에만 사용.
     */
    Mesh(std::vector<spJoint>& joints, const a::gl::core::spMeshGL meshGL);
    
    /**
     * @brief Copy constructor for not-skinned mesh.
     */
    Mesh(const Mesh& other);

    /**
     * @brief Copy constructor for skinned mesh. m_joints는 새로운 joint로 replace
     */
    Mesh(std::vector<spJoint>& joints, const Mesh& other);
    
    ~Mesh() = default;

    /**
     * @brief Set the materials object
     */
    void set_materials(const std::vector<Material>& materials);

    /**
     * @brief Update m_buffer
     */
    void update_mesh();
    
    int  vertex_num() const;
    bool use_skinning() const;

private:
    friend class Render;
    
    /**
     * @brief m_mesh는 객체가 아님. 따라서 delete 하지 말 것.
     */
    const a::gl::core::spMeshGL m_meshGL;

    /**
     * @brief materials
     */
    std::vector<Material>       m_materials;
    
    /**
     * @brief skinning 된 mesh 인지 아닌지. 같은 mesh라도 skinning 사용하는 것과 사용하지 않는 것은 다른 객체로 취급하기.
     */
    const bool                  m_use_skinning;
    
    /**
     * @brief pointer to the joints
     */
    std::vector<spJoint>        m_joints;
    std::map<std::string, int>  m_jnt_name_to_idx;

    /**
     * @brief buffer_trf = world_jnt_trf * jnt_bind_trf.inv
     */
    std::vector<glm::mat4>      m_buffer;

    /**
     * @brief 각 m_joints 에 해당하는 buffer index
     */
    std::vector<int>            m_jnt_buffer_idx;
};

}
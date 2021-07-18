#pragma once
#include "eigentype.h"
#include "material.h"
#include "motion.h"
#include <memory>
#include <map>

namespace a::gl {
namespace core {
class MeshGL;
using spMeshGL = std::shared_ptr<MeshGL>;
}

class Joint;
class Mesh;
class Model;
using spJoint = std::shared_ptr<Joint>;
using spMesh  = std::shared_ptr<Mesh>;
using spModel = std::shared_ptr<Model>;

/**
 * @brief AGL 모델 정보. 보통 shared pointer로 사용.
 *        생성은 FBX 클래스에서. 직접 생성은 비추.
 */
class Model
{
public:    
    /**
     * @brief mesh가 없는 경우. skinning X. joints are not copied.
     */
    explicit Model(std::vector<spJoint>& joints);
    
    /**
     * @brief root joint를 자동으로 생성. skinning X.
     */
    explicit Model(const std::vector<std::pair<core::spMeshGL, vMaterial>>& gl_meshes);

    /**
     * @brief gl_meshes에서 mesh 객체 새로 생성. skinning O. joints are not copied.
     */
    Model(std::vector<spJoint>& joints, 
          const std::vector<std::pair<core::spMeshGL, vMaterial>>& gl_meshes);
    
    /**
     * @brief joints and meshes are not copied. skinning O
     */
    Model(std::vector<spJoint>& joints, std::vector<spMesh>& meshes);
    
    ~Model() = default;
    
    spModel                     copy() const;

    // meshes
    void                        update_mesh();              // update mesh buffer. update 하지 않을 경우, mesh가 올바르게 rendering 안됨.
    int                         mesh_number() const;        // 하나의 model은 여러개의 mesh를 가질 수 있음.
    spMesh                      mesh(int mesh_id);
    const spMesh                mesh(int mesh_id) const;
    std::vector<spMesh>         meshes();
    const std::vector<spMesh>   meshes() const;
    
    // joints
    spJoint                     root();
    const spJoint               root() const;
    std::vector<spJoint>        joints();
    const std::vector<spJoint>  joints() const;
    spJoint                     joint(const std::string& name);
    const spJoint               joint(const std::string& name) const;
    std::vector<spJoint>        joints(const std::vector<std::string>& names);
    const std::vector<spJoint>  joints(const std::vector<std::string>& names) const;

    void                        set_pose(const Pose& pose); // pose와 m_joints의 정보 순서가 반드시 같아야 함.
    void                        set_pose(const Pose& pose, const std::vector<std::string>& names);

    // 현재 pose의 rotation 값들을 pre transformation 에 저장.
    void                        set_current_pose_identity_rot();
    
private:
    /**
     * @brief index 0 is always root joint
     */
    std::vector<spJoint>       m_joints;
    std::map<std::string, int> m_jnt_name_to_idx;
    std::vector<spMesh>        m_meshes;
};

}
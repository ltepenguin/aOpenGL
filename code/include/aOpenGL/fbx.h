#pragma once
#include "motion.h"
#include "material.h"
#include <memory>
#include <string>

namespace a::gl {
namespace core {
struct MeshGL;
using spMeshGL = std::shared_ptr<MeshGL>;
}

class FBXParser;
class Model;
class Mesh;
class Joint;
using spModel = std::shared_ptr<Model>;
using spMesh  = std::shared_ptr<Mesh>;
using spJoint = std::shared_ptr<Joint>;

/**
 * @brief import fbx
 */
class FBX 
{
public:
    explicit FBX(std::string path, float scale = 0.01);

    // check copy constructor (todo)
    FBX(const FBX&) = delete;
    FBX& operator=(const FBX&) = delete;
    
    ~FBX();
    
    std::vector<spJoint> joints();
    
    /**
     * @brief 이 함수로 리턴된 mesh는 skinning 사용 X
     */
    std::vector<spMesh> meshes();

    /**
     * @brief model.
     */
    spModel model();

    std::vector<Motion> motion(spModel model);    
    
    /**
     * @brief motion 정보는 jnts의 name 순서대로 가져옴.
     */
    std::vector<Motion> motion(const std::vector<spJoint>& jnts);

private:
    /**
     * @return meshGL and assigned materials
     */
    std::vector<std::pair<core::spMeshGL, vMaterial>> meshGLs();

    struct Parser;
    std::unique_ptr<Parser> m_parser;
    
    float m_scale;
};

}
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <memory>
#include <glad/glad.h>

namespace a::gl {
namespace core {

/**
 * @brief Vertex Array Object
 */
struct VAO
{
    GLuint vao, vbo, ebo;
    int idx_num;
};

/**
 * @brief Common vertex attributes
 */
struct VertexGL
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec3 material_id;
    glm::vec4 skinning_idxes;
    glm::vec4 skinning_weights;
};

/**
 * @brief MeshGL
 */
struct MeshGL
{
    VAO vao;

    std::vector<VertexGL>      vertices;
    std::vector<unsigned int>  indices;

    /**
     * @brief world_vtx_pos = world_jnt_trf * local_vtx_pos
     *                      = world_jnt_trf * jnt_bind_trf.inv * vtx_bind_pos
     *                      = buffer_trf * vtx_bind_pos
     */
    bool is_skinned;
    std::vector<std::string>   joint_order;
    std::map<std::string, int> name_to_idx;
    std::vector<glm::mat4>     jonit_bind_trf_inv;
    
    //std::vector<Material>      materials;
};
using spMeshGL = std::shared_ptr<MeshGL>;

/**
 * @return OpenGL vertex array object 만들고 id 정보들 리턴
 */
VAO bind_mesh(std::vector<VertexGL>& varray, std::vector<unsigned int>& indices);

/**
 * @brief tangent와 bitangent를 uv를 활용하여 계산. 만약 uv가 discontinuous 하다면 사용하지 말것.
 *        이 함수는 사용 x.
 * @param out_tan    output
 * @param out_bitan  output
 * @param positions  input vertex positions
 * @param normals    input vertex normals
 * @param uvs        input vertex uvs
 * @param indices    input vertex indices
 */
void compute_tangent_space(std::vector<glm::vec3>& out_tan, 
                           std::vector<glm::vec3>& out_bitan,
                           const std::vector<glm::vec3>& positions, 
                           const std::vector<glm::vec3>& normals,
                           const std::vector<glm::vec2>& uvs,
                           const std::vector<unsigned int>& indices);

std::vector<VertexGL> to_vertex_array(std::vector<glm::vec3>& positions, 
                                      std::vector<glm::vec3>& normals,
                                      std::vector<glm::vec2>& uvs,
                                      std::vector<glm::vec3>& tangents,
                                      std::vector<glm::vec3>& bitangents,
                                      std::vector<unsigned int>& indices,
                                      float scale = 1.0f);

std::vector<VertexGL> to_vertex_array(std::vector<glm::vec3>& positions, 
                                      std::vector<glm::vec3>& normals,
                                      std::vector<glm::vec2>& uvs,
                                      std::vector<glm::vec3>& tangents,
                                      std::vector<glm::vec3>& bitangents,
                                      std::vector<unsigned int>& indices,
                                      std::vector<glm::ivec4>& lbs_indices,
                                      std::vector<glm::vec4>& lbs_weights,
                                      float scale = 1.0f);

}
}
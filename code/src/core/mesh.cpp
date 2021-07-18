#include "aOpenGL/core/mesh.h"

namespace a::gl::core {

VAO bind_mesh(std::vector<VertexGL>& varray, std::vector<unsigned int>& indices)
{
    GLuint vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexGL) * varray.size(), &varray[0], GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (void*)0);

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (void*)offsetof(VertexGL, normal));
    
    // uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (void*)offsetof(VertexGL, uv));

    // tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (void*)offsetof(VertexGL, tangent));

    // bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (void*)offsetof(VertexGL, bitangent));

    // material ID
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (void*)offsetof(VertexGL, material_id));

    // skinning joint ids
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (void*)offsetof(VertexGL, skinning_idxes));

    // skinning joint weights
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (void*)offsetof(VertexGL, skinning_weights));

    // copy index data to ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    VAO meshVAO;
    meshVAO.vao = vao;
    meshVAO.vbo = vbo;
    meshVAO.ebo = ebo;    
    meshVAO.idx_num = (int)indices.size();
    return meshVAO;
}

void compute_tangent_space(std::vector<glm::vec3>& out_tan, 
                           std::vector<glm::vec3>& out_bitan,
                           const std::vector<glm::vec3>& positions, 
                           const std::vector<glm::vec3>& normals,
                           const std::vector<glm::vec2>& uvs,
                           const std::vector<unsigned int>& indices)
{
    out_tan.clear();
    out_bitan.clear();

    int vnum = (int)positions.size();
    out_tan.resize(vnum, glm::vec3(0, 0, 0));
    out_bitan.resize(vnum, glm::vec3(0, 0, 0));

    for(int idx = 0; idx < indices.size(); idx+=3)
    {
        int vidx0 = indices.at(idx + 0);
        int vidx1 = indices.at(idx + 1);
        int vidx2 = indices.at(idx + 2);

        const auto& v0 = positions[vidx0];
        const auto& v1 = positions[vidx1];
        const auto& v2 = positions[vidx2];

        const auto& uv0 = uvs[vidx0];
        const auto& uv1 = uvs[vidx1];
        const auto& uv2 = uvs[vidx2];

        auto delta_pos1 = v1 - v0;
        auto delta_pos2 = v2 - v0;

        auto delta_uv1 = uv1 - uv0;
        auto delta_uv2 = uv2 - uv0;
        
        float r = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
        glm::vec3 tangent = (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) * r;
        glm::vec3 bitangent = (delta_pos2 * delta_uv1.x - delta_pos1 * delta_uv2.x) * r;
        
        out_tan.at(vidx0) += tangent;
        out_tan.at(vidx1) += tangent;
        out_tan.at(vidx2) += tangent;
        
        out_bitan.at(vidx0) += bitangent;
        out_bitan.at(vidx1) += bitangent;
        out_bitan.at(vidx2) += bitangent;
    }

    for(int i = 0; i < vnum; ++i)
    {
        out_tan.at(i) = glm::normalize(out_tan.at(i));
        out_bitan.at(i) = glm::normalize(out_bitan.at(i));
    }
}

std::vector<VertexGL> to_vertex_array(std::vector<glm::vec3>& positions, 
                                    std::vector<glm::vec3>& normals,
                                    std::vector<glm::vec2>& uvs,
                                    std::vector<glm::vec3>& tangents,
                                    std::vector<glm::vec3>& bitangents,
                                    std::vector<unsigned int>& indices,
                                    float scale)
{
    int vnum = positions.size();
    std::vector<VertexGL> varray(vnum);
    for(int i = 0; i < vnum; ++i)
    {
        varray.at(i).position = scale * positions.at(i);
        varray.at(i).normal = normals.at(i);
        varray.at(i).uv = uvs.at(i);
        varray.at(i).tangent = tangents.at(i);
        varray.at(i).bitangent = bitangents.at(i);
        varray.at(i).material_id = glm::vec3(0, 0, 0);
        // lbs is not used
        varray.at(i).skinning_idxes = glm::vec4(0, 0, 0, 0);
        varray.at(i).skinning_weights = glm::vec4(0, 0, 0, 0);
    }
    return varray;
}

std::vector<VertexGL> to_vertex_array(std::vector<glm::vec3>& positions, 
                                    std::vector<glm::vec3>& normals,
                                    std::vector<glm::vec2>& uvs,
                                    std::vector<glm::vec3>& tangents,
                                    std::vector<glm::vec3>& bitangents,
                                    std::vector<unsigned int>& indices,
                                    std::vector<glm::ivec4>& lbs_indices,
                                    std::vector<glm::vec4>& lbs_weights,
                                    float scale)
{
    int vnum = positions.size();
    std::vector<VertexGL> varray(vnum);
    for(int i = 0; i < vnum; ++i)
    {
        varray.at(i).position = scale * positions.at(i);
        varray.at(i).normal = normals.at(i);
        varray.at(i).uv = uvs.at(i);
        varray.at(i).tangent = tangents.at(i);
        varray.at(i).bitangent = bitangents.at(i);
        varray.at(i).material_id = glm::vec3(0, 0, 0);
        varray.at(i).skinning_idxes = lbs_indices.at(i);
        varray.at(i).skinning_weights = lbs_weights.at(i);
    }
    return varray;
}

}
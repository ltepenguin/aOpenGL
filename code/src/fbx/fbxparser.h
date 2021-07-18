#pragma once
#include <fbxsdk.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <map>
#include <vector>
#include "keyframe.h"

namespace a::gl {

struct TextureParseInfo;
struct MaterialParseInfo;
struct MeshData;
struct CharacterData;
struct SkinningData;

/**
 * @brief fbx parser
 * @author ckm
 * @since Thu Aug 27 2020
 */
class FBXParser
{
public:
    explicit FBXParser(std::string path);

    ~FBXParser() { clear(); }
    
    void mesh_data(std::vector<MeshData>&, float scale);

    void character_data(CharacterData& data, float scale);

    void keyframes(std::vector<spSceneKeyFrames>& data, float scale);

private:
    void clear();

    ::fbxsdk::FbxManager* m_manager;
    
    ::fbxsdk::FbxScene*   m_scene;
};

/**
 * @brief lbs data
 */
struct SkinningData
{
    std::map<std::string, int> name2idx;
    
    // offset과 buffer의 순서.
    std::vector<std::string> joint_order;
    
    // binding 할 때 joint transformation의 inverse
    std::vector<glm::mat4> offset_transform; 
    
    // skinning 계산 한 후의 transformation.
    // bufferTransform = jointGlobalTransform * offsetTransform
    // vertexPosition = bufferTransform * bindingGlobalVertexPosition
    //                = jointGlobalTransform * offsetTransform * bindingGlobalVertexPosition
    //std::vector<glm::mat4> buffer_transform; 

    // 각 vertex들의 index와 weight 값
    std::vector<glm::ivec4>    joint_indices1;
    std::vector<glm::ivec4>    joint_indices2;
    std::vector<glm::vec4>     joint_weights1;    
    std::vector<glm::vec4>     joint_weights2;
};

/**
 * @brief parsed mesh info
 */
struct MeshData
{
    std::multimap<int, int> controlPointIdx2vertexId;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    
    bool is_skinned;
    SkinningData skinning_data;

    std::vector<MaterialParseInfo> materials;
    std::vector<TextureParseInfo>  textures;

    std::vector<int> polygonMaterialConnection;
};

/**
 * @brief node data
 */
struct JointData
{
    std::string name;
    int parent_index;
    
    glm::vec3 local_T;
    glm::vec3 local_S; // currently, local_S is not used
    glm::quat local_R;
    glm::quat pre_R;
};

/**
 * @brief parsed character info
 */
struct CharacterData
{
    std::string name;
    std::vector<JointData> joint_data;
};

/**
 * @brief texture info
 * @author 
 * @since Thu Aug 27 2020
 */
struct TextureParseInfo
{
    // ================================================================================================================= //
    // alpha_src:
    //     "None", "RGB Intensity", "Black"
    // mapping_type:
    //     "Null", "Planar", "Spherical", "Cylindrical", "Box", "Face", "UV", "Environment"
    // blend_mode:
    //     "Translucent", "Additive", "Modulate", "Modulate2", "Over", "Normal", "Dissolve", "Darken", "ColorBurn", "LinearBurn",
    //     "DarkerColor", "Lighten", "Screen", "ColorDodge", "LinearDodge", "LighterColor", "SoftLight", "HardLight", "VividLight",
    //     "LinearLight", "PinLight", "HardMix", "Difference", "Exclusion", "Substract", "Divide", "Hue", "Saturation", "Color",
    //     "Luminosity", "Overlay"
    // material_use:
    //     "Model Material", "Default Material"
    // texture_use:
    //     "Standard", "Shadow Map", "Light Map", "Spherical Reflexion Map", "Sphere Reflexion Map", "Bump Normal Map"
    // ================================================================================================================= //
    std::string name                   {""};
    std::string fileName               {""};
    double      scale_u                {1.00};
    double      scale_v                {1.00};
    double      translation_u          {0.00};
    double      translation_v          {0.00};
    bool        swap_uv                {false};
    double      rotation_u             {0.00};
    double      rotation_v             {0.00};
    double      rotation_w             {0.00};
    std::string alpha_src              {"None"};
    double      crop_left              {0.00};
    double      crop_top               {0.00};
    double      crop_right             {0.00};
    double      crop_bottom            {0.00};
    std::string mapping_type           {""};
    std::string planner_mapping_normal {""};
    std::string blend_mode             {""};
    double      alpha                  {0.00}; 
    std::string material_use           {""};
    std::string texture_use            {""};
    
    // ================================================================================================================= //
    // property
    //   "DiffuseColor", "EmissiveColor", "SpecularColor", "ShininessExponent", "NormalMap"
    // ================================================================================================================= //
    std::string property               {""};
    int         _connected_material    {-1}; // fbx parser
    
    bool is_used;
};

/**
 * @brief material
 */
struct MaterialParseInfo
{
    std::string  name;
    glm::vec3    ambient;
    glm::vec3    diffuse;
    glm::vec3    specular;
    glm::vec3    emissive;
    double       opacity;
    double       shininess;
    double       reflectivity;
    int          material_id; // internal use
    std::string  type;        // Phong or Lambert

    std::vector<int> textureIDs;
};


// helpers

namespace fbx {

// see fbx_material.cpp
std::vector<MaterialParseInfo> getMaterials(FbxGeometry* pGeometry);

// see fbx_material.cpp
std::vector<int> getPolygonMaterialConnection(FbxMesh* pMesh);

// see fbx_texture.cpp
std::vector<TextureParseInfo> getTextures(FbxGeometry* pGeometry);

// see fbx_skin.cpp
bool getSkinning(SkinningData& data, FbxGeometry* pGeometry, std::multimap<int, int>& ctrlIdx2vtxIdx, int vertex_num, float scale);

}

}
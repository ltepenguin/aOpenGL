#include "aOpenGL/fbx.h"

#include "aOpenGL/model.h"
#include "aOpenGL/joint.h"
#include "aOpenGL/mesh.h"
#include "aOpenGL/material.h"

#include "aOpenGL/core/mesh.h"
#include "fbx/fbxparser.h"
#include <iostream>

namespace a::gl {

static std::map<std::string, a::gl::TextureType> fbx_property_names = 
{
    { "DiffuseColor",      a::gl::TextureType::kDiffuse },
    { "EmissiveColor",     a::gl::TextureType::kEmissive },
    { "SpecularColor",     a::gl::TextureType::kSpecular },
    { "ShininessExponent", a::gl::TextureType::kGlossiness },
    { "NormalMap",         a::gl::TextureType::kNormal }
};

static a::gl::TextureType find_texture_type(std::string type_name)
{
    auto iter = fbx_property_names.find(type_name);
    if(iter == fbx_property_names.end())
        return a::gl::TextureType::kUnknown;
    else
        return iter->second;
}

static std::vector<float> get_timestep(float t0, float te, float dt)
{
    std::vector<float> timestep;
    int nof = ((te - t0) / dt + 1);
    timestep.reserve(nof);
    for(int i = 0; i < nof; ++i)
    {
        float t = i * dt + t0;
        timestep.push_back(t);
    }
    return timestep;
}

struct FBX::Parser
{
    FBXParser parser;
    CharacterData charData;
    std::vector<MeshData> meshData;

    Parser(const std::string& path, float scale):
        parser(path), charData(), meshData()
    {
        parser.mesh_data(meshData, scale);
        parser.character_data(charData, scale);
    }
};

FBX::FBX(std::string path, float scale)
{
    m_parser = std::make_unique<FBX::Parser>(path, scale);
    m_scale = scale;
}

FBX::~FBX()
{}

std::vector<std::pair<core::spMeshGL, vMaterial>> FBX::meshGLs()
{
    auto& meshData = m_parser->meshData;
    
    std::vector<std::pair<core::spMeshGL, vMaterial>> results;
    int n = meshData.size();
    results.reserve(n);

    for(auto& data : meshData)
    {
        auto mesh_gl = std::make_shared<core::MeshGL>();

        if(data.is_skinned)
        {
            mesh_gl->is_skinned = true;
            mesh_gl->vertices = a::gl::core::to_vertex_array(
                data.positions,
                data.normals,
                data.uvs,
                data.tangents,
                data.bitangents,
                data.indices,
                data.skinning_data.joint_indices1,
                data.skinning_data.joint_weights1,
                1.0);
            
            mesh_gl->joint_order = data.skinning_data.joint_order;
            mesh_gl->name_to_idx = data.skinning_data.name2idx;
            mesh_gl->jonit_bind_trf_inv = data.skinning_data.offset_transform;
        }
        else
        {
            mesh_gl->is_skinned = false;
            mesh_gl->vertices = a::gl::core::to_vertex_array(
                data.positions,
                data.normals,
                data.uvs,
                data.tangents,
                data.bitangents,
                data.indices,
                1.0);
        }
#if 1
        // set materials and texture
        std::map<int, int> id_to_materialIdx;
        std::vector<Material> gl_materials;
        for(int i = 0; i < data.materials.size(); ++i)
        {
            MaterialParseInfo& material_info = data.materials.at(i);
            id_to_materialIdx[material_info.material_id] = i;
            
            Material gl_material;
            gl_material.albedo = material_info.diffuse;

            for(int j = 0; j < material_info.textureIDs.size(); ++j)
            {
                int tid = material_info.textureIDs.at(j);
                auto texture_info = data.textures.at(tid);
                
                auto gl_texture = TextureLoader::load(texture_info.fileName);
                auto gl_texturetype = find_texture_type(texture_info.property);
                gl_material.set_texture(gl_texturetype, gl_texture);
            }

            gl_materials.push_back(gl_material);
        }

        // set vertex material connection

        for(int i = 0; i < data.polygonMaterialConnection.size(); i ++)
        {   
            int mid  = data.polygonMaterialConnection.at(i);
            mid = id_to_materialIdx[mid];
            int idx0 = data.indices.at(i * 3 + 0);
            int idx1 = data.indices.at(i * 3 + 1);
            int idx2 = data.indices.at(i * 3 + 2);

            mesh_gl->vertices.at(idx0).material_id = glm::vec3(mid, mid, mid);
            mesh_gl->vertices.at(idx1).material_id = glm::vec3(mid, mid, mid);
            mesh_gl->vertices.at(idx2).material_id = glm::vec3(mid, mid, mid);
        }
#if 0
        mesh->materials = gl_materials;
        
        // material이 없을 경우, 기본 material로 setting
        if(mesh->materials.size() == 0)
            mesh->materials.push_back(Material());
#endif

#endif
        mesh_gl->indices = data.indices;
        mesh_gl->vao = a::gl::core::bind_mesh(mesh_gl->vertices, mesh_gl->indices);
        
        results.push_back(
            {mesh_gl, gl_materials}
        );
    }
    return results;
}

std::vector<spJoint> FBX::joints()
{
    std::vector<spJoint> joints;

    // get data
    //CharacterData data;
    //m_parser->character_data(data, m_scale);
    auto& charData = m_parser->charData;

    // generate objects
    std::vector<JointData>& jnts = charData.joint_data;
    std::vector<int> parent_idxes;
    for(int i = 0; i < jnts.size(); ++i)
    {
        JointData& jnt = jnts.at(i);
        auto obj = std::make_shared<Joint>();
        obj->set_name(jnt.name);
        obj->set_local_rot(to_eigen(jnt.local_R));
        obj->set_local_pos(to_eigen(jnt.local_T));
        obj->set_scale(to_eigen(jnt.local_S));
        obj->set_pre_rot(to_eigen(jnt.pre_R));
        parent_idxes.push_back(jnt.parent_index);
        joints.push_back(obj);
    }

    // link objects
    for(int i = 0; i < joints.size(); ++i)
    {
        int pidx = parent_idxes.at(i);
        if(pidx < 0)
            continue;

        auto parent = joints.at(pidx);
        auto child = joints.at(i);
        parent->add_child(child);
        child->set_parent(parent);
    }
    return joints;
}

std::vector<spMesh> FBX::meshes()
{
    std::vector<spMesh> meshes;
    auto mesh_gls = this->meshGLs();
    for(auto mesh_gl : mesh_gls)
    {
        auto mesh = std::make_shared<Mesh>(mesh_gl.first);
        mesh->set_materials(mesh_gl.second);
        meshes.push_back(mesh);
    }
    return meshes;
}

spModel FBX::model()
{
    auto jnts = this->joints();
    auto mesh_gls = this->meshGLs();
    
    bool mesh_exist = (mesh_gls.size() > 0);
    bool jnt_exist = (jnts.size() > 0);
    
    if(mesh_exist && jnt_exist)
    {
        return std::make_shared<Model>(jnts, mesh_gls);
    }
    else if(mesh_exist)
    {
        return std::make_shared<Model>(mesh_gls);
    }
    else if(jnt_exist)
    {
        return std::make_shared<Model>(jnts);
    }

    assert(mesh_exist || jnt_exist);
    return nullptr;
}

std::vector<Motion> FBX::motion(spModel model)
{
    return this->motion(model->joints());
}

std::vector<Motion> FBX::motion(const std::vector<spJoint>& jnts)
{
    namespace keyframe = ::a::gl::keyframe;
    auto& parser = m_parser->parser;

    // original scenes
    std::vector<spSceneKeyFrames> scenes;
    parser.keyframes(scenes, m_scale);

    // resampled scenes
    std::vector<spSceneKeyFrames> resampled_scenes;
    resampled_scenes.reserve(scenes.size());

    // timesteps
    std::vector<std::vector<float>> timestep_set;
    timestep_set.reserve(scenes.size());

    std::vector<std::string> names;
    int noj = jnts.size();
    names.reserve(noj);
    for(int i = 0; i < noj; ++i)
    {
        names.push_back(jnts.at(i)->name());
    }

    static float dt = 1.0 / 60.0;
    for(auto scene : scenes)
    {
        //std::cout << scene->name << ": " << scene->start_time << " ~ " << scene->end_time << std::endl;
        std::vector<float> timestep = get_timestep(scene->start_time, scene->end_time, dt);
        timestep_set.push_back(timestep);
        resampled_scenes.push_back(keyframe::resample(scene, timestep));
    }

    std::vector<Motion> m_set;
    m_set.reserve(resampled_scenes.size());
    std::string root_name = jnts.at(0)->name();
    for(int i = 0; i < resampled_scenes.size(); ++i)
    {
        auto scene = resampled_scenes.at(i);
        int nof = timestep_set.at(i).size();
        auto rotations_set = keyframe::get_rotations_from_resampled(names, scene, nof);

        Motion m;
        m.name = scene->name;
        m.poses.resize(nof);
        m.start_time = scene->start_time;
        m.end_time = scene->end_time;
        
        auto positions = keyframe::get_translations_from_resampled(root_name, scene);
        for(int i = 0; i < nof; ++i)
        {
            Pose& p = m.poses.at(i);
            
            p.root_position = (positions.size() > i) ? to_eigen(positions.at(i)) : Vec3::Zero();
            p.local_rotations.reserve(noj);
            for(int j = 0; j < noj; ++j)
            {
                p.local_rotations.push_back(
                    to_eigen(rotations_set.at(j).at(i))
                );
            }
        }

        m_set.push_back(m);
    }
    return m_set;
}

}
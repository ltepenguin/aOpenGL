#include "aOpenGL/renderoption.h"
#include "aOpenGL/joint.h"
#include <iostream>

namespace a::gl {

RenderOptions::RenderOptions(core::VAO vao, 
                             core::Shader* shader, 
                             void (*fpDraw)(spRenderOptions)):
    m_vao(vao),
    m_shader(shader),
    m_position(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_orientation(glm::mat3(1.0f)),
    m_scale(glm::vec3(1.0f, 1.0f, 1.0f)),
    m_materials({Material()}),
    m_disp_map_scale(0.01f),
    m_uv_repeat(1.0f),
    m_draw_floor_grid(false),
    m_debug(false),
    m_use_skinning(false),
    m_buffer_transforms(),
    m_fpDraw(fpDraw)
{
}

void RenderOptions::draw()
{
    this->m_fpDraw(shared_from_this());
}

spRenderOptions RenderOptions::position(Vec3 pos)
{
    this->m_position = a::gl::to_glm(pos);
    return shared_from_this();
}

spRenderOptions RenderOptions::position(float* xyz)
{
    this->m_position = glm::vec3(xyz[0], xyz[1], xyz[2]);
    return shared_from_this();
}

spRenderOptions RenderOptions::position(float x, float y, float z)
{
    this->m_position = glm::vec3(x, y, z);
    return shared_from_this();
}

spRenderOptions RenderOptions::orientation(Mat3 R)
{
    this->m_orientation = a::gl::to_glm(R);
    return shared_from_this();
}

spRenderOptions RenderOptions::transform(Mat4 m4)
{
#if 0
    auto trf = a::gl::to_glm(m4);
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(trf, scale, rotation, translation, skew, perspective);
    
    this->m_orientation = glm::mat3(rotation);
    this->m_scale       = scale;
    this->m_position    = translation;
#else
    this->m_orientation = a::gl::to_glm((Mat3)m4.block<3, 3>(0, 0));
    this->m_position    = a::gl::to_glm((Vec3)m4.col(3).head<3>());
#endif
    return shared_from_this();
}

spRenderOptions RenderOptions::scale(Vec3 S)
{
    this->m_scale = a::gl::to_glm(S);
    return shared_from_this();
}

spRenderOptions RenderOptions::scale(float s)
{
    this->m_scale = glm::vec3(s, s, s);
    return shared_from_this();
}

spRenderOptions RenderOptions::scale(float x, float y, float z)
{
    this->m_scale = glm::vec3(x, y, z);
    return shared_from_this();
}

spRenderOptions RenderOptions::scale(float* xyz)
{
    this->m_scale = glm::vec3(xyz[0], xyz[1], xyz[2]);
    return shared_from_this();
}

spRenderOptions RenderOptions::attach_to(spJoint jnt)
{
    this->m_position = a::gl::to_glm(jnt->world_pos());
    this->m_orientation = a::gl::to_glm(jnt->world_rot_mat());
    return shared_from_this();
}

spRenderOptions RenderOptions::color(Vec3 clr, int mid)
{
    if(this->m_materials.size() == 0)
    {
        this->m_materials.push_back(Material());
        mid = 0;
    }

    if(mid < m_materials.size())
    {
        this->m_materials.at(mid).albedo = a::gl::to_glm(clr);
        this->m_materials.at(mid).albedo_map.handle = 0;
    }
    return shared_from_this();
}

spRenderOptions RenderOptions::color(float r, float g, float b, int mid)
{
    return this->color(Vec3(r, g, b), mid);
}

spRenderOptions RenderOptions::alpha(float a)
{
    int n = this->m_materials.size();
    for(int i = 0; i < n; ++i)
        this->alpha(a, i);
    return shared_from_this();
}

spRenderOptions RenderOptions::alpha(float a, int mid)
{
    if(this->m_materials.size() == 0)
    {
        this->m_materials.push_back(Material());
        mid = 0;
    }
    
    if(mid < m_materials.size())
    {   
        this->m_materials.at(mid).alpha = a;
    }
    return shared_from_this();
}

spRenderOptions RenderOptions::metallic(float v, int mid)
{
    if(mid < m_materials.size())
    {
        this->m_materials.at(mid).metallic = v;
        this->m_materials.at(mid).metallic_map.handle = 0;
    }
    return shared_from_this();
}

spRenderOptions RenderOptions::roughness(float v, int mid)
{
    if(mid < m_materials.size())
    {
        this->m_materials.at(mid).roughness = v;
        this->m_materials.at(mid).roughness_map.handle = 0;
    }
    return shared_from_this();
}

spRenderOptions RenderOptions::texture(std::string path, TextureType type, int mid)
{
    if(this->m_materials.size() == 0)
    {
        this->m_materials.push_back(Material());
        mid = 0;
    }
    
    if(mid < m_materials.size())
    {
        this->m_materials.at(mid).set_texture(type, TextureLoader::load(path));
    }

    return shared_from_this();
}

spRenderOptions RenderOptions::texture_repeat(float n)
{
    m_uv_repeat = n;
    return shared_from_this();
}

spRenderOptions RenderOptions::disp_scale(float scale)
{
    this->m_disp_map_scale = scale;
    return shared_from_this();
}

spRenderOptions RenderOptions::floor_grid(bool is_floor)
{
    this->m_draw_floor_grid = is_floor;
    return shared_from_this();
}

spRenderOptions RenderOptions::debug(bool is_debug)
{
    this->m_debug = is_debug;
    return shared_from_this();
}

// ****** RenderOptionsVec ****** //

RenderOptionsVec::RenderOptionsVec(std::vector<spRenderOptions>& list)
: m_render_list(list)
{
}

void RenderOptionsVec::draw()
{
    for(auto& ro : m_render_list)
        ro->draw();
}

spRenderOptionsVec RenderOptionsVec::orientation(Mat3 m3)
{
    for(auto& ro : m_render_list)
        ro->orientation(m3);
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::position(Vec3 v3)
{
    for(auto& ro : m_render_list)
        ro->position(v3);
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::scale(float scale)
{
    for(auto& ro : m_render_list)
        ro->scale(scale);
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::transform(Mat4 trf)
{
    for(auto& ro : m_render_list)
        ro->transform(trf);
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::color(Vec3 v3)
{
    for(auto& ro : m_render_list)
        ro->color(v3);
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::color(float r, float g, float b)
{
    for(auto& ro : m_render_list)
        ro->color(r, g, b);
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::metallic(float v)
{
    for(auto& ro : m_render_list)
        ro->metallic(v);
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::roughness(float v)
{
    for(auto& ro : m_render_list)
        ro->roughness(v);
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::texture(std::string path, TextureType type, int mid )
{
    for(auto& ro : m_render_list)
    {
        ro->texture(path, type, mid);
    }
    return shared_from_this();
}

spRenderOptionsVec RenderOptionsVec::debug(bool is_debug)
{
    for(auto& ro : m_render_list)
    {
        ro->debug(true);
    }
    return shared_from_this();
}


}
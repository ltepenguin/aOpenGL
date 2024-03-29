#pragma once
#include <memory>
#include "eigentype.h"
#include "core/mesh.h"
#include "core/shader.h"
#include "material.h"

namespace a::gl {

class RenderOptions;
class RenderOptionsVec;
class Joint;
using spRenderOptions = std::shared_ptr<RenderOptions>;
using spRenderOptionsVec = std::shared_ptr<RenderOptionsVec>;
using spJoint = std::shared_ptr<Joint>;

/**
 * @brief 사용 예시: Render::cube()->position(pos)->draw()
 * @author ckm 
 */
class RenderOptions : public std::enable_shared_from_this<RenderOptions>
{
public:
    RenderOptions(core::VAO vao, 
                  core::Shader* shader, 
                  core::Shader* alpha_shader, 
                  void (*fpDraw)(spRenderOptions, core::Shader*));
   
    void draw();
  
    spRenderOptions position(Vec3);
    spRenderOptions position(float* xyz);
    spRenderOptions position(float x, float y, float z);
    spRenderOptions orientation(Mat3);
    spRenderOptions transform(Mat4);
    spRenderOptions scale(Vec3);
    spRenderOptions scale(float);
    spRenderOptions scale(float x, float y, float z);
    spRenderOptions scale(float* xyz);
    spRenderOptions attach_to(spJoint);
    spRenderOptions color(Vec3 clr, int mid = 0);    
    spRenderOptions color(float r, float g, float b, int mid = 0);
    spRenderOptions alpha(float a);
    spRenderOptions alpha(float a, int mid);
    spRenderOptions metallic(float, int mid = 0);
    spRenderOptions roughness(float, int mid = 0);
    spRenderOptions texture(std::string, TextureType type = TextureType::kAlbedo, int mid = 0);
    spRenderOptions texture_repeat(float n);
    spRenderOptions disp_scale(float scale = 0.01f);
    spRenderOptions floor_grid(bool use_grid, float line_width = 1.0f, float line_interval = 1.0f, Vec3 line_color = Vec3(0.0f, 0.0f, 0.0f));
    spRenderOptions debug(bool);

private:
    bool is_transparent();
    void alpha_draw();

private:   
    friend class Render;
    friend class RenderOptionsVec;

    // basic rendering information
    core::VAO              m_vao;
    core::Shader*          m_shader;
    core::Shader*          m_alpha_shader;

    // transformation
    glm::vec3              m_position;
    glm::mat3              m_orientation;
    glm::vec3              m_scale;
    
    // material options
    std::vector<Material>  m_materials;
    float                  m_disp_map_scale;
    float                  m_uv_repeat;

    // floor grid
    bool                   m_draw_floor_grid;
    glm::vec3              m_grid_color;
    float                  m_grid_width;
    float                  m_grid_interval;
    
    // debug mode
    bool                   m_debug;

    // skinning option
    bool                   m_use_skinning;
    std::vector<glm::mat4> m_buffer_transforms;

    // text
    std::string            m_text;
    float                  m_line_space;

    // final drawing function pointer
    void (*m_fpDraw)(spRenderOptions, core::Shader* shader);
};

/**
 * @brief redner options for multiple vao
 */
class RenderOptionsVec : public std::enable_shared_from_this<RenderOptionsVec>
{
public:
    explicit RenderOptionsVec(std::vector<spRenderOptions>& list);
    
    void draw();

    spRenderOptionsVec orientation(Mat3);
    spRenderOptionsVec position(Vec3);
    spRenderOptionsVec scale(float);
    spRenderOptionsVec transform(Mat4);
    spRenderOptionsVec color(Vec3);
    spRenderOptionsVec color(Vec3 clr, int mid);
    spRenderOptionsVec color(float r, float g, float b);
    spRenderOptionsVec metallic(float);
    spRenderOptionsVec roughness(float);
    spRenderOptionsVec texture(std::string, TextureType type = TextureType::kAlbedo, int mid = 0);
    spRenderOptionsVec debug(bool);
    spRenderOptionsVec alpha(float);

private:
    friend class Render;
    std::vector<spRenderOptions> m_render_list;
};

}
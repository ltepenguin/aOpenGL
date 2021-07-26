#include "aOpenGL/render.h"

#include "aOpenGL/app.h"
#include "aOpenGL/camera.h"
#include "aOpenGL/joint.h"
#include "aOpenGL/light.h"
#include "aOpenGL/mesh.h"
#include "aOpenGL/model.h"
#include "aOpenGL/renderoption.h"
#include "aOpenGL/core/primitive.h"

#include "aOpenGL/config.h"

#include <iostream>

namespace a::gl {

// shadow mode
Render::RenderMode Render::render_type;

// shaders
core::Shader* Render::primitive_shader;
core::Shader* Render::lbs_shader;
core::Shader* Render::shadow_shader;

core::Shader* Render::alpha_primitive_shader;
core::Shader* Render::alpha_lbs_shader;

// shadows
unsigned int Render::depth_map_fbo;
unsigned int Render::depth_map_handle;

// environment
core::Shader* Render::tocube_shader;
core::Shader* Render::background_shader;

// app render info
struct Render::AppRenderInfo
{
    glm::vec4 sky_color{glm::vec4(0.8f, 0.8f, 0.82f, 1.0f)};
    glm::vec3 cam_position;
    glm::mat4 cam_projection;
    glm::mat4 cam_view;
    glm::vec3 light_direction{0.25f, 1.0f, 0.5f};
    glm::vec3 light_color{50.0f, 50.0f, 50.0f};
    glm::mat4 light_space;
};
std::shared_ptr<Render::AppRenderInfo> Render::app_render_info;

// Functions *** //

#define AGL_RETURN_PBR_RENDER_OPTIONS(PRIMITIVE) \
    if(render_type == Render::RenderMode::SHADOW) { \
        return std::make_shared<RenderOptions>( \
            RenderOptions(PRIMITIVE, Render::shadow_shader, nullptr, Render::draw_shadow)); \
    } \
    else { \
        return std::make_shared<RenderOptions>( \
            RenderOptions(PRIMITIVE, Render::primitive_shader, Render::alpha_primitive_shader, Render::draw_pbr)); \
    }

spRenderOptions Render::cube()
{
    AGL_RETURN_PBR_RENDER_OPTIONS(core::VAOPrimitive::cube());
}

spRenderOptions Render::sphere()
{
    AGL_RETURN_PBR_RENDER_OPTIONS(core::VAOPrimitive::sphere());
}

spRenderOptions Render::plane()
{
    AGL_RETURN_PBR_RENDER_OPTIONS(core::VAOPrimitive::plane());
}

spRenderOptions Render::cylinder()
{
    AGL_RETURN_PBR_RENDER_OPTIONS(core::VAOPrimitive::cylinder());
}

spRenderOptions Render::cone()
{
    AGL_RETURN_PBR_RENDER_OPTIONS(core::VAOPrimitive::cone());
}

spRenderOptions Render::pyramid()
{
    AGL_RETURN_PBR_RENDER_OPTIONS(core::VAOPrimitive::pyramid());
}

spRenderOptions Render::mesh(spMesh m)
{
    spRenderOptions ro;

    if(m->m_use_skinning)
    {
        if(render_type == Render::RenderMode::SHADOW)
        {
            ro = std::make_shared<RenderOptions>(
                RenderOptions(m->m_meshGL->vao, Render::shadow_shader, nullptr, Render::draw_shadow)
            );
        }
        else
        {
            ro = std::make_shared<RenderOptions>(
                RenderOptions(m->m_meshGL->vao, Render::lbs_shader, Render::alpha_lbs_shader, Render::draw_pbr)
            );
        }        
        
        // set buffer
        ro->m_use_skinning = true;
        ro->m_buffer_transforms = m->m_buffer;
    }
    else
    {
        if(render_type == Render::RenderMode::SHADOW)
        {
            ro = std::make_shared<RenderOptions>(
                RenderOptions(m->m_meshGL->vao, Render::shadow_shader, nullptr, Render::draw_shadow)
            );
        }
        else
        {
            ro = std::make_shared<RenderOptions>(
                RenderOptions(m->m_meshGL->vao, Render::primitive_shader, Render::alpha_primitive_shader, Render::draw_pbr)
            );
        }
    }
    
    ro->m_materials = m->m_materials;
    return ro;
}

spRenderOptionsVec Render::model(spModel m, bool update_mesh)
{
    if(update_mesh)
        m->update_mesh();

    auto meshes = m->meshes();
    std::vector<spRenderOptions> rov;
    rov.reserve(m->mesh_number());

    for(auto mesh : meshes)
    {
        rov.push_back(Render::mesh(mesh));
    }

    return std::make_shared<RenderOptionsVec>(rov);
}

spRenderOptionsVec Render::skeleton(spModel model)
{
    std::vector<spRenderOptions> ro_v;
    int noj = model->joints().size();
    ro_v.reserve(noj);

    auto jnts = model->joints();
    for(auto& jnt : jnts)
    {
        if(jnt->parent() == nullptr)
            continue;
        
        auto trf = jnt->skel_world_trf();
        float scale_y = jnt->skel_length();
        float scale_xz = 0.1f * scale_y;
        auto ro = Render::pyramid()->transform(trf)->scale(scale_xz, scale_y, scale_xz)->debug(true);
        ro_v.push_back(ro);
    }
    
    auto options = std::make_shared<RenderOptionsVec>(ro_v);
    return options;
}

static void generate_shadow_buffer(GLuint& fbo, GLuint& shadow_map, int reolustion)
{
    // configure depth map FBO
    // -----------------------
    glGenFramebuffers(1, &fbo);
    
    // create depth texture
    glGenTextures(1, &shadow_map);
    glBindTexture(GL_TEXTURE_2D, shadow_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, reolustion, reolustion, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    // reset the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static std::string absolute_path(const char* path)
{
    static const std::string agl_path(AGL_PATH);
    return agl_path + std::string(path);
}

void Render::initialize_shaders()
{
    // pbr shader initialize
    Render::primitive_shader 
        = new core::Shader(absolute_path(AGL_PBR_VS), absolute_path(AGL_PBR_FS));
    Render::primitive_shader->build();

    // lbs shader initialize
    Render::lbs_shader 
        = new core::Shader(absolute_path(AGL_LBS_PBR_VS), absolute_path(AGL_PBR_FS));
    Render::lbs_shader->build();

    // pbr shader initialize
    Render::alpha_primitive_shader 
        = new core::Shader(absolute_path(AGL_PBR_VS), absolute_path(AGL_EMPTY_FS));
    Render::alpha_primitive_shader->build();

    // lbs shader initialize
    Render::alpha_lbs_shader 
        = new core::Shader(absolute_path(AGL_LBS_PBR_VS), absolute_path(AGL_EMPTY_FS));
    Render::alpha_lbs_shader->build();

    // IBL map initialize
    Render::tocube_shader 
        = new core::Shader(absolute_path(AGL_TOCUBE_VS), absolute_path(AGL_TOCUBE_FS));
    Render::tocube_shader->build();
    TextureLoader::load_envmap(absolute_path(AGL_BACKGROUND_HDR_PATH), Render::tocube_shader);

    // background map
    //Render::background_shader = new Shader(AGL_BACKGROUND_VS, AGL_BACKGROUND_FS);
    //Render::background_shader->build();

    // shader map initialize
    Render::shadow_shader 
        = new core::Shader(absolute_path(AGL_SHADOW_VS), absolute_path(AGL_EMPTY_FS));
    Render::shadow_shader->build();
    generate_shadow_buffer(Render::depth_map_fbo, Render::depth_map_handle, AGL_SHADOW_MAP_SIZE);

    // set app render info
    app_render_info = std::make_shared<AppRenderInfo>();
}

void Render::set_render_mode(Render::RenderMode type, int width, int height)
{
    if(type == Render::RenderMode::SHADOW)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, Render::depth_map_fbo);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    Render::render_type = type;
}

glm::vec4 Render::sky_color()
{
    return Render::app_render_info->sky_color;
}

void Render::set_sky_color(float r, float g, float b)
{
    Render::app_render_info->sky_color = glm::vec4(r, g, b, 1.0f);
}

void Render::set_sky_color(Vec3 rgb)
{
    Render::app_render_info->sky_color = glm::vec4(to_glm(rgb), 1.0f);
}

void Render::update_render_view(App* app, int width, int height)
{
    const auto& cam = app->camera();
    const auto& light = app->light();
    
    app_render_info->cam_position = cam.position_gl();
    if(cam.is_perspective())
    {
        app_render_info->cam_projection = glm::perspective(glm::radians(cam.zoom()), (float)width / (float)height, 0.1f, 1000.0f);
    }
    else
    {
        float scale = 0.00001f * cam.ortho_zoom();
        app_render_info->cam_projection = glm::ortho(-scale * width, scale * width, -scale * height, scale * height, -100.0f, 1000.0f);
    }
    app_render_info->cam_view = cam.view_matrix_gl();
    app_render_info->light_direction = light.direction;
    app_render_info->light_color = light.intensity * light.color;
    app_render_info->light_space = light.light_space_matrix();

    static std::vector<core::Shader*> shader_list = 
    {
        Render::primitive_shader, Render::lbs_shader, 
        Render::alpha_primitive_shader, Render::alpha_lbs_shader
    };
    
    for(auto shader : shader_list)
    {
        if(shader)
            shader->view_update(false);
    }
}

void Render::draw_pbr(spRenderOptions option, core::Shader* shader)
{
    if(shader == nullptr)
        return;
    
    if(shader == nullptr)
        return;
    
    shader->use();
    if(shader->view_update() == false)
    {
        shader->setMat4("u_projection",     Render::app_render_info->cam_projection);
        shader->setMat4("u_view",           Render::app_render_info->cam_view);
        shader->setVec3("u_viewPosition",   Render::app_render_info->cam_position);
        shader->setVec3("u_lightDirection", Render::app_render_info->light_direction);
        shader->setVec3("u_lightColor",     Render::app_render_info->light_color);
        shader->setMat4("u_lightSpace",     Render::app_render_info->light_space);
        shader->setVec3("u_skyColor",       Render::app_render_info->sky_color);
        shader->view_update(true);
    }

    if(option->m_use_skinning)
    {
        shader->setMultipleMat4("u_lbs_joints", 
                                option->m_buffer_transforms.size(), 
                                &option->m_buffer_transforms[0]);
    }
    else
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0), option->m_position) * 
                              glm::mat4(option->m_orientation) * 
                              glm::scale(glm::mat4(1.0), option->m_scale);
        shader->setMat4("u_model", transform);
    }

    // texture indexing
    if(shader->texture_update() == false)
    {
        shader->setInt("u_irradianceMap", 0); // environment color
        shader->setInt("u_shadowMap",     1); // shadow

        // textures
        for(int i = 0; i < AGL_MAX_MATERIAL_TEXTURES; ++i)
        {
            std::string postfix = "[" + std::to_string(i) + "]";
            shader->setInt("u_textures" + postfix, 2 + i); // start from the second
        }
        shader->texture_update(true);
    }

    // set environment map
    {   
        Texture env_map = TextureLoader::load_envmap(
            absolute_path(AGL_BACKGROUND_HDR_PATH), Render::tocube_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, env_map.handle);
    }

    // set shadow map
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Render::depth_map_handle);
    }

    // remove all textures
    for(int i = 0; i < AGL_MAX_MATERIAL_TEXTURES; ++i)
    {
        glActiveTexture(GL_TEXTURE2 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // material settings
    {
        const std::vector<Material>& materials = option->m_materials;

        std::vector<glm::vec4> rgba(AGL_MAX_MATERIAL_NUM, glm::vec4(1, 1, 1, 1));
        std::vector<glm::vec3> attribs(AGL_MAX_MATERIAL_NUM, glm::vec3(0, 0, 0)); // metallic, roughness, none
        
        std::vector<glm::ivec3> isGS(AGL_MAX_MATERIAL_NUM, glm::ivec3(0, 0, 0)); // texture uses glossiness / specular
        std::vector<glm::ivec4> textureID1(AGL_MAX_MATERIAL_NUM, glm::ivec4(-1, -1, -1, -1));
        std::vector<glm::ivec3> textureID2(AGL_MAX_MATERIAL_NUM, glm::ivec3(-1, -1, -1));

        auto gl_set_texture = [](GLuint handle, int& idx, int& cnt) -> void
        {
            if((handle == 0) || (cnt >= AGL_MAX_MATERIAL_TEXTURES))
            {
                idx = -1;
                return;
            }

            idx = cnt;
            glActiveTexture(GL_TEXTURE2 + cnt);
            glBindTexture(GL_TEXTURE_2D, handle);
            cnt++;
        };

        int texture_cnt = 0;
        for(int i = 0; i < materials.size(); ++i)
        {
            if(i >= AGL_MAX_MATERIAL_NUM) 
                break;
          
            const Material& material = materials.at(i);
            rgba.at(i) = glm::vec4(material.albedo, material.alpha);
            attribs.at(i) = glm::vec3(material.metallic, material.roughness, 0);
            isGS.at(i).x = material.is_glossiness_map;
            isGS.at(i).x = material.is_specular_map;

            // textures
            gl_set_texture(material.albedo_map.handle,       textureID1.at(i).x, texture_cnt);
            gl_set_texture(material.normal_map.handle,       textureID1.at(i).y, texture_cnt);
            gl_set_texture(material.metallic_map.handle,     textureID1.at(i).z, texture_cnt);
            gl_set_texture(material.emissive_map.handle,     textureID1.at(i).w, texture_cnt);
            
            gl_set_texture(material.roughness_map.handle,    textureID2.at(i).x, texture_cnt);
            gl_set_texture(material.ao_map.handle,           textureID2.at(i).y, texture_cnt);
            gl_set_texture(material.displacement_map.handle, textureID2.at(i).z, texture_cnt);
        }

        shader->setMultipleVec4(      "u_mat_color", AGL_MAX_MATERIAL_NUM,       &rgba[0]);
        shader->setMultipleVec3(     "u_mat_attrib", AGL_MAX_MATERIAL_NUM,    &attribs[0]);
        shader->setMultipleIvec3(  "u_mat_isGS_txt", AGL_MAX_MATERIAL_NUM,       &isGS[0]);
        shader->setMultipleIvec4("u_mat_textureID1", AGL_MAX_MATERIAL_NUM, &textureID1[0]);
        shader->setMultipleIvec3("u_mat_textureID2", AGL_MAX_MATERIAL_NUM, &textureID2[0]);

        shader->setFloat("u_uv_scale", option->m_uv_repeat);
        shader->setFloat("u_dispMapScale", option->m_disp_map_scale);
        shader->setBool("u_floor_grid", option->m_draw_floor_grid);
        shader->setBool("u_debug", option->m_debug);
    }

    // Final rendering
    {
        glBindVertexArray(option->m_vao.vao);
        glDrawElements(GL_TRIANGLES, option->m_vao.idx_num, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Render::draw_shadow(spRenderOptions option, core::Shader* shader)
{
    if(shader == nullptr)
        return;
    
    if(shader == nullptr) 
        return;

    shader->use();
    
    // set light space
    shader->setMat4("u_lightSpace", Render::app_render_info->light_space);
    
    // set model matrix
    if(option->m_use_skinning)
    {
        shader->setBool("u_use_lbs", true);
        shader->setMultipleMat4("u_lbs_joints", 
                                option->m_buffer_transforms.size(), 
                                &option->m_buffer_transforms[0]);
    }
    else
    {
        shader->setBool("u_use_lbs", false);
        glm::mat4 transform = glm::translate(glm::mat4(1.0), option->m_position) * 
                              glm::mat4(option->m_orientation) * 
                              glm::scale(glm::mat4(1.0), option->m_scale);
        shader->setMat4("u_model", transform);
    }

    // Final rendering
    glBindVertexArray(option->m_vao.vao);
    glDrawElements(GL_TRIANGLES, option->m_vao.idx_num, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

}
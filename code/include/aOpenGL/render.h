#pragma once
#include "appmanager.h"
#include "eigentype.h"
#include <memory>
#include <glm/glm.hpp>

namespace a::gl {
namespace core {
class Shader;
}

class App;
class Mesh;
class Model;
class RenderOptions;
class RenderOptionsVec;
using spMesh = std::shared_ptr<Mesh>;
using spModel = std::shared_ptr<Model>;
using spRenderOptions = std::shared_ptr<RenderOptions>;
using spRenderOptionsVec = std::shared_ptr<RenderOptionsVec>;

/**
 * @brief Render class
 */
class Render
{
public:
    static spRenderOptions    cube();
    static spRenderOptions    sphere();
    static spRenderOptions    plane();

    // todo: normal 확인하기.
    static spRenderOptions    cylinder();
    static spRenderOptions    cone();
    static spRenderOptions    pyramid();

    static spRenderOptions    mesh(spMesh);
    static spRenderOptionsVec model(spModel);
    static spRenderOptionsVec skeleton(spModel);

    static void set_sky_color(float r, float g, float b);
    static void set_sky_color(Vec3 rgb);

private:
    /**
     * App manager에서 Render 관리. private 함수들 call.
     */
    friend class AppManager;

    /**
     * @brief initialize all the shaders. 단 한번만 call 할 것.
     */
    static void initialize_shaders();
    
    /**
     * @brief app manager에서 shadow 그리기 전후로 부를것
     */
    static void set_shadow_mode(bool use_shadow, int width, int height);

    /**
     * @brief get background color
     */
    static glm::vec4 sky_color();

    /**
     * @brief shader 안의 uniform parameter 들을 app 정보와 동기화 하도록 함.
     *        항상 Draw를 사용하기 전에 이 함수를 call 할 것.
     */
    static void update_render_view(App* app, int width, int height);

    /**
     * @brief pbr rendering function
     */
    static void draw_pbr(spRenderOptions option);
    
    /**
     * @brief shadow rendering function
     */
    static void draw_shadow(spRenderOptions option);

private:
    // shadow mode
    static bool use_shadow;

    // shaders
    static core::Shader* primitive_shader;
    static core::Shader* lbs_shader;
    static core::Shader* shadow_shader;

    // shadows
    static unsigned int depth_map_fbo;
    static unsigned int depth_map_handle;

    // environment
    static core::Shader* tocube_shader;
    static core::Shader* background_shader;

    // app info
    struct AppRenderInfo;
    static std::shared_ptr<AppRenderInfo> app_render_info;
};

}
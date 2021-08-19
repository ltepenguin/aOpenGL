#include "aOpenGL/appmanager.h"
#include "aOpenGL/render.h"
#include "aOpenGL/app.h"
#include "aOpenGL/image.h"
#include "aOpenGL/config.h"
#include "aOpenGL/file.h"

#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <ctime>
#include <filesystem>

namespace a::gl {

App* AppManager::app{nullptr};

void AppManager::initialize()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_SAMPLES, 4); // (testing)
}

void AppManager::set_app(App* app)
{
    AppManager::app = app;
}

// not used
#if 0
static std::string cur_time()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    std::string str(buffer);
    return str;
}
#endif

static Image::spData capture_screen()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];
    unsigned char *data = (unsigned char*) malloc((size_t) (width * height * 3)); // 3 components (R, G, B)
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    auto screen = std::make_shared<Image::Data>();
    screen->path = "";
    screen->image = data;
    screen->width = width;
    screen->height = height;
    screen->channel = 3;
    return screen;
}

void AppManager::start_loop()
{
    if(AppManager::app == nullptr)
    {
        std::cout << "AppManager::app is empty." << std::endl;
        return;
    }
    
    const char* title = "app";
    auto window = glfwCreateWindow(app->width(), app->height(), title, nullptr, nullptr);
    if(!window)
        throw std::runtime_error {
            "Failed to create GLFW window"
        };

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    glfwSetFramebufferSizeCallback(window, AppManager::on_resize);
    glfwSetKeyCallback(window, AppManager::on_key_down);
    glfwSetCursorPosCallback(window, AppManager::on_mouse_move);
    glfwSetMouseButtonCallback(window, AppManager::on_mouse_button_click);
    glfwSetScrollCallback(window, AppManager::on_scroll);
    glfwSetErrorCallback(AppManager::on_error);

    // glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }    
    
    // configure global opengl state
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
    }

    // initialize all the shaders
    {
        ::a::gl::Render::initialize_shaders();
        glViewport(0, 0, app->width(), app->height());
    }

    // start
    {
        AppManager::app->start();
    }

    // main loop

    while(!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // sky color
        {
            auto sclr = a::gl::Render::sky_color();
            glViewport(0, 0, width, height);
            glClearColor(sclr.x, sclr.y, sclr.z, sclr.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        
        // update
        {
            ::a::gl::AppManager::app->update();
        }
        
        // update camera & lights
        {
            ::a::gl::Render::update_render_view(AppManager::app, width, height);
        }

        // shadow mode
        {
            // set viewport
            ::a::gl::Render::set_render_mode(Render::RenderMode::SHADOW, width, height);
            glViewport(0, 0, AGL_SHADOW_MAP_SIZE, AGL_SHADOW_MAP_SIZE);
            glClear(GL_DEPTH_BUFFER_BIT);
            ::a::gl::AppManager::app->render();            
        }

        // render
        {
            ::a::gl::Render::set_render_mode(Render::RenderMode::PBR, width, height);
            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ::a::gl::AppManager::app->render();
        }
        
        // render environment map
        {
            //::a::gl::Render::background();
        }

        // render xray
        {
            ::a::gl::Render::set_render_mode(Render::RenderMode::PBR, width, height);
            glClear(GL_DEPTH_BUFFER_BIT);
            ::a::gl::AppManager::app->render_xray();
        }
        
        // late update
        {
            ::a::gl::AppManager::app->late_update();
        }

        // event
        {
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // screen capture
        {
            if(app->capture())
            {
                static int shot = 0;
                auto scene = capture_screen();
                if(file_check(app->capture_path()) == false)
                    std::filesystem::create_directories(app->capture_path());
                Image::save_image(app->capture_path() + std::to_string(shot) + ".png", scene, true);
                shot++;
            }
        }
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void AppManager::terminate()
{
}

void AppManager::on_key_down(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(app) app->key_callback(window, key, scancode, action, mods);
}

void AppManager::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
    if(app) app->mouse_callback(window, xpos, ypos);
}

void AppManager::on_mouse_button_click(GLFWwindow* window, int button, int action, int mods)
{
    if(app) app->mouse_button_callback(window, button, action, mods);
}

void AppManager::on_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if(app) app->scroll_callback(window, xoffset, yoffset);
}

void AppManager::on_error(int error, const char* desc)
{
    if(app) app->on_error(error, desc);
}

void AppManager::on_resize(GLFWwindow* window, int width, int height)
{
    if(app) app->on_resize(window, width, height);
}

}
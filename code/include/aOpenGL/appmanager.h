#pragma once
#pragma GCC system_header
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace a {
namespace gl {

class App;

/**
 * @brief simple app cycle manager. namespace 처럼 사용하면 됨.
 */
class AppManager
{
public:
    static void start(App* app) 
    {
        AppManager::initialize();
        AppManager::set_app(app);
        AppManager::start_loop();
    }

    static void initialize();
    static void set_app(App* app);
    static void start_loop();
    static void terminate();
private:
    static void on_key_down(GLFWwindow* window, int key, int scancode, int action, int mods);   
    static void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
    static void on_mouse_button_click(GLFWwindow* window, int button, int action, int mods);
    static void on_scroll(GLFWwindow* window, double xoffset, double yoffset);
    static void on_error(int error, const char* desc);   
    static void on_resize(GLFWwindow* window, int width, int height);

    static App* app;
    static bool do_capture;
};

}
}
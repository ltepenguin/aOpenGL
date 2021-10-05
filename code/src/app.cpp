#include "aOpenGL/app.h"

namespace a::gl {

struct App::IO
{
    bool   doCapture{false};
    double lastMouseX;
    double lastMouseY;
};

App::App(): m_camera(), m_light(), m_capture(false), m_width(1920), m_height(1080), m_capture_path("capture/"), _io(new App::IO)
{}

App::~App()
{}

void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if(key == GLFW_KEY_F2 && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if(key == GLFW_KEY_F5 && action == GLFW_PRESS)
    {
        this->capture(true);
    }
    if(key == GLFW_KEY_F6 && action == GLFW_PRESS)
    {
        this->capture(false);
    }

    key_callback(key, action);

    const char* keyName = glfwGetKeyName(key, 0);
    if(keyName != nullptr)
    {
        key_callback(keyName[0], action);
    }
}

void App::key_callback(char key, int action)
{}

void App::key_callback(int key, int action)
{}

void App::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    float xoffset = xpos - _io->lastMouseX;
    float yoffset = _io->lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

    _io->lastMouseX = xpos;
    _io->lastMouseY = ypos;

    bool left_shift_pressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    bool left_alt_pressed   = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
    bool mouse_left_pressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    bool mouse_right_pressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    if (left_shift_pressed && mouse_left_pressed)
    {
        m_camera.mouse_move(xoffset, yoffset);
    }
    else if(mouse_left_pressed)
    {
        m_camera.mouse_input(xoffset, yoffset);
    }
    else if (mouse_right_pressed)
    {
        m_camera.mouse_move(xoffset, yoffset);
    }
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_camera.mouse_scroll_input(yoffset);
}

void App::on_error(int error, const char* desc)
{
    
}

void App::on_resize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);    
    m_width = width;
    m_height = height;
}

}
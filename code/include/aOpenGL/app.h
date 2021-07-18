#pragma once
#pragma GCC system_header
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "camera.h"
#include "light.h"

namespace a::gl {

class App {
public:
   App();
   virtual ~App();

   // override these functions
   
   virtual void start(){}
   virtual void update(){}
   virtual void late_update(){}
   virtual void render(){}
   virtual void render_xray(){}
   virtual void key_callback(char key, int action);

   // opengl key call back functions

   void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);   
   virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos);
   virtual void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
   virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
   virtual void on_error(int error, const char* desc);
   virtual void on_resize(GLFWwindow* window, int width, int height);

   // get
   
   Camera& camera() { return m_camera; }
   Light& light() { return m_light; }
   int width() { return m_width; }
   int height() { return m_height; }

   // set
   void set_window_size(int width, int height) { m_width = width; m_height = height; }

   // capture

   bool capture() { return m_capture; }
   void capture(bool set) { m_capture = set; }

private:
   Camera m_camera;
   Light  m_light;
   bool   m_capture;
   int    m_width;
   int    m_height;

   struct IO;
   std::unique_ptr<App::IO> _io;
};

}
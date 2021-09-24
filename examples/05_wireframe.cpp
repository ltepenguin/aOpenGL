#include <aOpenGL.h>

class MyApp : public agl::App
{
public:
    float angle = 0.0f;

    void update() override
    {
        angle += 0.01f;
        
        if(angle > M_PI)
            angle -= (2.0f * M_PI);
        
        camera().set_perspective(false);
        camera().set_position(glm::vec3(0, 0, 3));
        camera().set_focus(glm::vec3(0, 0, 0));
    }

    void render() override
    {
        Quat q0(AAxis(angle, Vec3::UnitY()));
        
        // draw textured sphere 
        agl::Render::sphere()
            ->orientation(q0.toRotationMatrix())
            ->draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        agl::Render::sphere()
            ->color(0, 0, 0)
            ->orientation(q0.toRotationMatrix())
            ->draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void key_callback(char key, int action) override
    {
        if(action != GLFW_PRESS)
            return;
        
        if(key == '1')
            this->capture(true);
        if(key == '2')
            this->capture(false);
    }
    
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
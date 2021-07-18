#include <aOpenGL.h>

class MyApp : public agl::App
{
public:
    const char* p_color     = "../data/textures/Tiles012_2K-JPG/Tiles012_2K_Color.jpg";
    const char* p_normal    = "../data/textures/Tiles012_2K-JPG/Tiles012_2K_Normal.jpg";
    const char* p_roughness = "../data/textures/Tiles012_2K-JPG/Tiles012_2K_Roughness.jpg";

    float angle = 0.0f;

    void update() override
    {
        angle += 0.01f;
        
        if(angle > M_PI)
            angle -= (2.0f * M_PI);
        
        camera().set_position(glm::vec3(0, 0, 3));
        camera().set_focus(glm::vec3(0, 0, 0));
    }

    void render() override
    {
        Quat q0(AAxis(angle, Vec3::UnitY()));
        
        // draw textured sphere 
        agl::Render::sphere()
            ->orientation(q0.toRotationMatrix())
            ->texture(p_color, agl::TextureType::kAlbedo)
            ->texture(p_normal, agl::TextureType::kNormal)
            ->texture(p_roughness, agl::TextureType::kRoughness)
            ->draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        agl::Render::sphere()
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
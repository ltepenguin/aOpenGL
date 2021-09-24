#include <aOpenGL.h>

class MyApp : public agl::App
{
public:
    const char* p_color     = "../data/textures/Metal032_1K-JPG/Metal032_1K_Color.jpg";
    const char* p_normal    = "../data/textures/Metal032_1K-JPG/Metal032_1K_NormalGL.jpg";
    const char* p_metallic  = "../data/textures/Metal032_1K-JPG/Metal032_1K_Metalness.jpg";
    const char* p_roughness = "../data/textures/Metal032_1K-JPG/Metal032_1K_Roughness.jpg";

    void start() override
    {
        // change the sky color
        agl::Render::set_sky_color(0.8f, 0.9f, 1.0f);
    }

    void render() override
    {
        // draw plane
        agl::Render::plane()
            ->scale(1500.0f)
            ->floor_grid(true)
            ->draw();

        // draw cylinder
        agl::Render::cylinder()
            ->position(-2.0f, 0.5f, 0)
            ->scale(0.5)
            ->draw();

        // draw textured sphere 
        agl::Render::sphere()
            ->position(1.0f, 1.0f, 1.0f)
            ->texture(p_color, agl::TextureType::kAlbedo)
            ->texture(p_normal, agl::TextureType::kNormal)
            ->texture(p_roughness, agl::TextureType::kRoughness)
            ->texture(p_metallic, agl::TextureType::kMetallic)
            ->draw();

        // draw textured cube
        a::gl::Render::cube()
            ->position(0.0f, 0.5f, 0.0f)
            ->texture(p_color, agl::TextureType::kAlbedo)
            ->texture(p_normal, agl::TextureType::kNormal)
            ->texture(p_roughness, agl::TextureType::kRoughness)
            ->texture(p_metallic, agl::TextureType::kMetallic)
            ->draw();
        
        // draw cone
        //   NOTE: rendering order is important for a transparent object
        agl::Render::cone()
            ->alpha(0.6f)
            ->color(1, 1, 0)
            ->position(-1.0f, 0.5f, 0)
            ->scale(0.5)
            ->draw();
    }
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
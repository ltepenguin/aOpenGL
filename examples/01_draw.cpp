#include <aOpenGL.h>

class MyApp : public agl::App
{
public:
    const char* p_color     = "../data/textures/Metal032_4K-JPG/Metal032_4K_Color.jpg";
    const char* p_normal    = "../data/textures/Metal032_4K-JPG/Metal032_4K_Normal.jpg";
    const char* p_metallic  = "../data/textures/Metal032_4K-JPG/Metal032_4K_Metalness.jpg";
    const char* p_roughness = "../data/textures/Metal032_4K-JPG/Metal032_4K_Roughness.jpg";

    const char* p_color2     = "../data/textures/Rock037_4K-JPG/Rock037_4K_Color.jpg";
    const char* p_normal2    = "../data/textures/Rock037_4K-JPG/Rock037_4K_Normal.jpg";
    const char* p_roughness2 = "../data/textures/Rock037_4K-JPG/Rock037_4K_Roughness.jpg";
    const char* p_disp2      = "../data/textures/Rock037_4K-JPG/Rock037_4K_Displacement.jpg";
    const char* p_ao2        = "../data/textures/Rock037_4K-JPG/Rock037_4K_AmbientOcclusion.jpg";

    void render() override
    {
        // draw plane
        agl::Render::plane()
            ->scale(10.0f)
            ->color(0.15f, 0.15f, 0.15f)
            ->floor_grid(true)
            ->draw();

        // draw cylinder
        agl::Render::cylinder()
            ->position(-2.0f, 0.5f, 0)
            ->scale(0.5)
            ->draw();

        // draw cone
        agl::Render::cone()
            ->position(-1.0f, 0.5f, 0)
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
    }
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
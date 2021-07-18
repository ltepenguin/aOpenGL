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

    const char* p_color3     = "../data/textures/Fabric036_4K-JPG/Fabric036_4K_Color.jpg";
    const char* p_normal3    = "../data/textures/Fabric036_4K-JPG/Fabric036_4K_Normal.jpg";
    const char* p_roughness3 = "../data/textures/Fabric036_4K-JPG/Fabric036_4K_Roughness.jpg";
    const char* p_disp3      = "../data/textures/Fabric036_4K-JPG/Fabric036_4K_Displacement.jpg";
    const char* p_ao3        = "../data/textures/Fabric036_4K-JPG/Fabric036_4K_AmbientOcclusion.jpg";

    const char* p_color4     = "../data/textures/Tiles058_2K-JPG/Tiles058_2K_Color.jpg";
    const char* p_normal4    = "../data/textures/Tiles058_2K-JPG/Tiles058_2K_Normal.jpg";
    const char* p_roughness4 = "../data/textures/Tiles058_2K-JPG/Tiles058_2K_Roughness.jpg";
    const char* p_disp4      = "../data/textures/Tiles058_2K-JPG/Tiles058_2K_Displacement.jpg";
    const char* p_metal4     = "../data/textures/Tiles058_2K-JPG/Tiles058_2K_Metalness.jpg";

    int frame = 0;
    void update() override
    {
        frame++;
    }

    void render() override
    {
        float scale = 1.0f + std::sin(0.01f * frame);
        
        float angle = frame * 0.005f;
        Quat rq(AAxis(angle, Vec3::UnitY()));
        
        agl::Render::plane()
            ->scale(1.0f)
            ->texture(p_color2, agl::TextureType::kAlbedo)
            ->texture(p_normal2, agl::TextureType::kNormal)
            ->texture(p_roughness2, agl::TextureType::kRoughness)
            ->texture(p_disp2, agl::TextureType::kDisplacement)
            ->texture(p_ao2, agl::TextureType::kAO)
            ->metallic(0.0f)
            ->disp_scale(0.01f)
            ->orientation(rq.toRotationMatrix())
            ->draw();
    }
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
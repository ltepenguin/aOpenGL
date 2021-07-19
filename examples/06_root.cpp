#include <aOpenGL.h>
#include <iostream>

class MyApp : public agl::App
{
public:
    agl::spModel model;
    float fixed_y;

    void start()
    {       
        const char* model_path = "../data/fbx/ybot/model/ybot.fbx";
        agl::FBX model_fbx(model_path);
        model = model_fbx.model();

        fixed_y = model->joint(0)->world_pos().y();
    }

    int frame = 0;
    void update() override
    {
        // Set root translaten
        {
            float angle = frame / 180.0f * M_PI;
            float x = std::sin(angle);
            float z = std::cos(angle);
            model->joint(0)->set_local_pos(Vec3(x, fixed_y, z));
        }

        // TODO: Rotate root orientation. Y-axis로 회전시켜보기
        {
            float angle = frame / 180.0f * M_PI;
        }

        frame++;
    }

    void render() override
    {
        agl::Render::plane()
            ->scale(15.0f)
            ->floor_grid(true)
            ->color(0.2f, 0.2f, 0.2f)
            ->draw();
        
        agl::Render::model(model)->draw();
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
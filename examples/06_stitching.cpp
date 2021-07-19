#include <aOpenGL.h>

class MyApp : public agl::App
{
public:
    agl::spModel model;
    std::vector<agl::Motion> motions;

    void start()
    {       
        const char* model_path  = "../data/fbx/kmodel/model/kmodel.fbx";
        const char* motion_path = "../data/fbx/kmodel/motion/ubi_sprint1_subject2.fbx";

        agl::FBX model_fbx(model_path);
        agl::FBX motion_fbx(motion_path);
        model   = model_fbx.model();
        motions = motion_fbx.motion(model);
    }

    void update() override
    {
    }

    void render() override
    {
        agl::Render::plane()
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
#include <aOpenGL.h>
#include <iostream>

class MyApp : public agl::App
{
public:
    agl::spModel model;
    std::vector<agl::Motion> motions;

    int frame = 0;

    void start() override
    {
        agl::FBX fbx("../data/fbx/ybot/model/ybot.fbx");
        model   = fbx.model();
        motions = fbx.motion(model);

        // Print joint names ------------------------------------//
        {
            std::vector<agl::spJoint> joints = model->joints();
            for(int i = 0; i < joints.size(); ++i)
            {
                std::cout << i << " joint : " << joints.at(i)->name() << std::endl;
            }
        }
    }

    void update() override
    {
    }

    void render() override
    {
        agl::Render::plane()
            ->scale(10.0f)
            ->floor_grid(true)
            ->draw();

        agl::Render::model(model)
            ->draw();
    }

    void render_xray() override
    {
        agl::Render::skeleton(model)
            ->color(0, 1, 1)
            ->draw();
    }
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
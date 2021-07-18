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
        //agl::FBX fbx("../data/fbx/nsm/anubis.fbx");
        agl::FBX fbx("../data/fbx/kmodel/model/kmodel.fbx");
        model   = fbx.model();
        motions = fbx.motion(model);
        model->update_mesh();
    }

    void update() override
    {
        // const auto& motion = motions.at(1);
        // const auto& pose = motion.poses.at(frame);
        // model->set_pose(pose);
        // model->update_mesh();
        // frame = (frame + 1) % (int)motion.poses.size();
    }

    void render() override
    {
        agl::Render::plane()
            ->scale(10.0f)
            ->color(0.15f, 0.15f, 0.15f)
            ->floor_grid(true)
            ->draw();

        agl::Render::model(model)
            ->draw();
    }

    void render_xray() override
    {
        agl::Render::skeleton(model)
            ->color(0.9, 0.9, 0)
            ->draw();
    }
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
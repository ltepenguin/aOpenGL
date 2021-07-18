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
        // const char* model_path  = "../data/fbx/kmodel/model/kmodel.fbx";
        // const char* motion_path = "../data/fbx/kmodel/motion/ubi_sprint1_subject2.fbx";

        const char* model_path  = "../data/fbx/nsm/anubis.fbx";
        const char* motion_path = "../data/fbx/nsm/locomotion.fbx";

        agl::FBX model_fbx(model_path);
        model = model_fbx.model();
       
        agl::FBX motion_fbx(motion_path);
        motions = motion_fbx.motion(model);
    }

    void update() override
    {
        const auto& motion = motions.at(0);
        const auto& pose = motion.poses.at(frame);
        model->set_pose(pose);
        model->update_mesh();
        frame = (frame + 1) % (int)motion.poses.size();
    }

    void render() override
    {
        agl::Render::plane()
            ->scale(15.0f)
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
#include <aOpenGL.h>
#include <iostream>

static agl::Pose interpolate(const agl::Pose& pose_a, const agl::Pose& pose_b, float weight_a)
{
    // TODO: Interpolate pose_a and pose_b ----------------------------- //
    //
    //
    // ----------------------------------------------------------------- //

    // Dummy code ------------------------------------------------------ //
    agl::Pose pose = pose_a;
    return pose;
    // ----------------------------------------------------------------- //
}

class MyApp : public agl::App
{
public:
    agl::spModel model;
    agl::Motion  motion;

    agl::Pose pose_a, pose_b;

    void start()
    {       
        const char* model_path  = "../data/fbx/ybot/model/ybot.fbx";
        const char* motion_path = "../data/fbx/ybot/motion/Running To Turn.fbx";

        agl::FBX model_fbx(model_path);
        agl::FBX motion_fbx(motion_path);
        
        model  = model_fbx.model();
        motion = motion_fbx.motion(model).at(0);

        pose_a = motion.poses.at(0);
        pose_b = motion.poses.back();
    }

    int frame = 0;
    void update() override
    {
        float weight = (frame % 300) / 300.0f;
        auto pose = interpolate(pose_a, pose_b, weight);
        model->set_pose(pose);
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
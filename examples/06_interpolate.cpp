#include <aOpenGL.h>
#include <iostream>

static agl::Pose interpolate(const agl::Pose& pose_a, const agl::Pose& pose_b, float weight_a)
{
#if 1
    agl::Pose newPose;
    for(int i=0 ; i<pose_a.local_rotations.size() ; i++){
        newPose.local_rotations.push_back(
            pose_a.local_rotations[i].slerp(weight_a, pose_b.local_rotations[i])
        );
    }
    newPose.root_position = pose_a.root_position * (1 - weight_a) + pose_b.root_position * weight_a;
    return newPose;
#else
    // TODO: Interpolate pose_a and pose_b ----------------------------- //
    // Dummy code ------------------------------------------------------ //
    agl::Pose pose = pose_b;
    return pose;
    // ----------------------------------------------------------------- //
#endif
}

class MyApp : public agl::App
{
public:
    agl::spModel model;
    agl::spModel modelA, modelB;
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

        modelA = model->copy();
        modelB = model->copy();
        modelA->set_pose(pose_a);
        modelB->set_pose(pose_b);
    }

    int frame = 0;
    void update() override
    {
        float weight = (frame % 300) / 300.0f;
        std::cout << weight << std::endl;
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
        agl::Render::model(modelA)->alpha(0.5)->draw();
        agl::Render::model(modelB)->alpha(0.5)->draw();
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
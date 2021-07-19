#include <aOpenGL.h>
#include <iostream>

class MyApp : public agl::App
{
public:
    agl::spModel model;
    agl::Motion  motion_a;
    agl::Motion  motion_b;
    std::vector<agl::Pose> pose_sequence;

    void start()
    {       
        const char* model_path    = "../data/fbx/ybot/model/ybot.fbx";
        const char* motion_path_a = "../data/fbx/ybot/motion/Running To Turn.fbx";
        const char* motion_path_b = "../data/fbx/ybot/motion/Running.fbx";

        agl::FBX model_fbx(model_path);
        agl::FBX motion_a_fbx(motion_path_a);
        agl::FBX motion_b_fbx(motion_path_b);
        
        model     = model_fbx.model();
        motion_a = motion_a_fbx.motion(model).at(0);
        motion_b = motion_b_fbx.motion(model).at(0);

        // TODO: Stitch two motions
        {
            pose_sequence = motion_a.poses;
            pose_sequence.insert(pose_sequence.end(), motion_b.poses.begin(), motion_b.poses.end());
        }
        
        // example
        {

        }
    }

    int frame = 0;
    void update() override
    {
        model->set_pose(pose_sequence.at(frame));
        frame = (frame + 1) % pose_sequence.size();
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
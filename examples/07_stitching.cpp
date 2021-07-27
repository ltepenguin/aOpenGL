#include <aOpenGL.h>
#include <iostream>

static std::vector<agl::Pose> stitch(
    const std::vector<agl::Pose>& poses_a, 
    const std::vector<agl::Pose>& poses_b)
{
    // TODO: Stitch two motions ---------------------------------------- //
    std::vector<agl::Pose> new_poses = poses_a;
    
    Vec3 v3 = poses_a.back().root_position;
    float x = v3.x();
    float y = v3.y();
    float z = v3.z();

    Vec3 last_root_pos = Vec3(x, 0, z);
    Quat last_root_orient = poses_a.back().local_rotations[0];
    
    // Compute inverse of first orientation of poses_b
    Quat inverse = poses_b[0].local_rotations[0].inverse();

    for(int i=0 ; i<poses_b.size() ; i++){
        agl::Pose newPose = poses_b[i];
        
        // Rotate root position vector to desired orientation
        newPose.root_position = last_root_orient * inverse * newPose.root_position;
        // Then translate the position
        newPose.root_position += last_root_pos;

        // Multiply inverse of first orientation
        // Then rotate by last root orientation of poses_a
        // Changing the base orientation to last frame of poses_a
        newPose.local_rotations[0] = last_root_orient * inverse * poses_b[i].local_rotations[0];

        new_poses.push_back(newPose);
    }


    return new_poses;

    // ----------------------------------------------------------------- //
    
    // Dummy code ------------------------------------------------------ //
    // std::vector<agl::Pose> new_poses = poses_a;
    // new_poses.insert(new_poses.end(), poses_b.begin(), poses_b.end());
    // return new_poses;
    // ----------------------------------------------------------------- //
}

class MyApp : public agl::App
{
public:
    agl::spModel            model;
    agl::Motion             motion_a;
    agl::Motion             motion_b;
    std::vector<agl::Pose>  stitched;

    void start()
    {       
        const char* model_path    = "../data/fbx/ybot/model/ybot.fbx";
        const char* motion_path_a = "../data/fbx/ybot/motion/Running To Turn.fbx";
        const char* motion_path_b = "../data/fbx/ybot/motion/Running.fbx";

        agl::FBX model_fbx(model_path);
        agl::FBX motion_a_fbx(motion_path_a);
        agl::FBX motion_b_fbx(motion_path_b);
        
        model    = model_fbx.model();
        motion_a = motion_a_fbx.motion(model).at(0);
        motion_b = motion_b_fbx.motion(model).at(0);

        // Rotate motion by 90 degrees
        Quat dq(AAxis(M_PI * 0.5f, Vec3::UnitY()));
        for(auto& pose : motion_a.poses)
        {
            pose.root_position = dq * pose.root_position;
            pose.local_rotations.at(0) = dq * pose.local_rotations.at(0);
        }


        stitched = stitch(motion_a.poses, motion_b.poses);
    }

    int frame = 0;
    void update() override
    {
        model->set_pose(stitched.at(frame));
        frame = (frame + 1) % stitched.size();

        //cam.set_position(Vec3);
        //cam.set_focus(Vec3);
        agl::spJoint root = model->joint(0);
        Vec3 pos = root->world_pos();
        pos.y() = 1.0f;
        this->camera().set_focus(pos);
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
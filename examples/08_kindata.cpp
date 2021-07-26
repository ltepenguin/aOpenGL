#include <aOpenGL.h>
#include <iostream>

// Configuration -------------------------------------------- //
const char* model_path  = "../data/fbx/kmodel/model/kmodel.fbx";
const char* motion_path = "../data/fbx/kmodel/motion/ubi_sprint1_subject2.fbx";
const std::vector<std::string> joint_names = {
    "Hips", 
    "LeftUpLeg", "LeftLeg", "LeftFoot", "LeftToeBase",
    "RightUpLeg", "RightLeg", "RightFoot", "RightToeBase",
    "Spine", "Spine1", "Spine2", 
    "LeftShoulder", "LeftArm", "LeftForeArm", "LeftHand", 
    "RightShoulder", "RightArm", "RightForeArm", "RightHand", 
    "Neck", "Head"
};

std::string left_shoulder  = "LeftShoulder";
std::string right_shoulder = "RightShoulder";
// ----------------------------------------------------------- //

class MyApp : public agl::App
{
public:
    // 렌더링을 위한 모델
    agl::spModel     model;
    
    // data 처리를 위한 모델
    agl::spKinModel  kmodel;
    agl::spKinMotion kmotion;

    void start() override
    {
        agl::FBX model_fbx(model_path);
        model = model_fbx.model();
       
        agl::FBX motion_fbx(motion_path);
        auto motions = motion_fbx.motion(model);
        
        kmodel = agl::kinmodel(model, joint_names);
        kmotion = agl::kinmotion(kmodel, motions);
        kmotion->init_world_basisTrf_from_shoulders(left_shoulder, right_shoulder);
        kmotion->apply_basisTrf_filter(3);
    }

    int frame = 0;
    agl::KinPose kpose;

    void update() override
    {
        int pidx = frame % kmotion->poses.size();
        kpose = kmotion->poses.at(pidx);

        model->set_pose(kpose, joint_names);
    }

    void render() override
    {
        for(auto trf : kpose.world_trfs)
        {
            agl::Render::cube()->transform(trf)
                ->color(1, 0, 0)->scale(0.05f)->draw();
        }

        agl::Render::cube()->transform(kpose.world_basisTrf)
            ->scale(0.1f)->color(0, 1, 0)->draw();

        agl::Render::plane()->scale(10.0f)->draw();

        agl::Render::model(model)->alpha(0.5f)->draw();
    }

    void late_update() override
    {
        frame++;
    }
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
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
    agl::spModel     model, model2;
    
    // data 처리를 위한 모델
    agl::spKinModel  kmodel;
    agl::spKinMotion kmotion;

    // displacements
    agl::KinPose simul_pose;
    std::vector<agl::KinDisp> disps;

    void start() override
    {
        agl::FBX model_fbx(model_path);
        model = model_fbx.model();
        model2 = model->copy();
       
        agl::FBX motion_fbx(motion_path);
        auto motions = motion_fbx.motion(model);
        
        kmodel = agl::kinmodel(model, joint_names);
        kmotion = agl::kinmotion(kmodel, motions);
        kmotion->init_world_basisTrf_from_shoulders(left_shoulder, right_shoulder);
        kmotion->apply_basisTrf_filter(3);

        // test displacements
        int nof = kmotion->poses.size();
        disps.resize(nof - 1);
        for(int i = 0; i < nof - 1; ++i)
        {
            disps.at(i) = agl::KinDisp(kmotion->poses.at(i), kmotion->poses.at(i + 1));
        }
        simul_pose = kmotion->poses.at(0);
        simul_pose.move_world_basisTrf(Mat4::Identity());
    }

    int frame = 0;
    agl::KinPose kpose;

    void update() override
    {
        // set model
        int pidx = frame % kmotion->poses.size();
        kpose = kmotion->poses.at(pidx);
        kpose.move_world_basisTrf(Mat4::Identity());
        model->set_pose(kpose, joint_names);

        // set model2
        simul_pose.add(disps.at(frame), kmodel);
        model2->set_pose(simul_pose, joint_names);
    }

    void render() override
    {
        agl::Render::plane()->scale(10.0f)->draw();
        agl::Render::model(model)->draw();
        agl::Render::model(model2)->draw();

    }

    void render_xray() override
    {
        agl::Render::cube()->transform(kpose.world_basisTrf)->debug(true)
            ->scale(0.1f)->color(0, 1, 0)->draw();
        for(auto trf : kpose.world_trfs)
        {
            agl::Render::cube()->transform(trf)->debug(true)
                ->color(1, 1, 0)->scale(0.05f)->draw();
        }

        agl::Render::cube()->scale(0.1f)->debug(true)
            ->position(kpose.local_pos.head<3>())->color(1, 0, 0)->draw();
        
        agl::Render::cube()->transform(simul_pose.world_basisTrf)->debug(true)
            ->scale(0.1f)->color(0, 0, 1)->draw();
        for(auto trf : simul_pose.world_trfs)
        {
            agl::Render::cube()->transform(trf)->debug(true)
                ->color(1, 0, 1)->scale(0.05f)->draw();
        }
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
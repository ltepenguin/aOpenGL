#include <aOpenGL.h>
#include <iostream>

class MyApp : public agl::App
{
public:
    agl::spModel model;
    std::vector<agl::Motion> motions;
    Vec3 cam_offset;
    Vec3 cube_offset;
    Mat4 cube_transform;
    Mat4 proj_transform;

    int frame = 0;

    void start() override{
        const char* model_path = "../data/fbx/kmodel/model/kmodel.fbx";
        const char* motion_path = "../data/fbx/kmodel/motion/ubi_sprint1_subject2.fbx";

        agl::FBX model_fbx(model_path);
        model = model_fbx.model();

        agl::FBX motion_fbx(motion_path);
        motions = motion_fbx.motion(model);

        // Set camera offset
        cam_offset = 4.0f * Vec3(0.0f, 1.0f, 1.0f);

        // Set cube offset
        cube_offset = Vec3(-1.0f, 1.0f, -1.0f);

        cube_transform = Mat4().Identity();
        proj_transform = Mat4().Identity();


        // Print motion name
        {
            std::cout << motion_path << " imported" << std::endl;
            std::cout << "number of motions: " << motions.size() << std::endl;
            for(int i=0 ; i<motions.size() ; i++)
            {
                std::cout << "motion " << i << std::endl;
                std::cout << "\tname : " << motions.at(i).name << std::endl;
                std::cout << "\tnumber of frames : " << motions.at(i).poses.size() << std::endl;
            }
        }
    }

    void update() override 
    {
        const auto& motion = motions.at(0);
        const auto& pose = motion.poses.at(frame);
        model->set_pose(pose);
        frame = (frame + 1) % (int)motion.poses.size();

        // Character's root position
        Vec3 root_pos = model->root()->world_pos();
        Mat3 root_rot = model->root()->world_rot_mat();

        /* Let camera follow the character */
        Vec3 cam_pos = root_pos + cam_offset;
        cam_pos.y() = 2.0f;
        camera().set_position(cam_pos);

        Vec3 cam_focus = root_pos;
        cam_focus.y() = 1.0f;
        camera().set_focus(cam_focus);


        /* Let cube follow character */
        // Get translation for the cube
        Vec3 cube_pos = root_pos + cube_offset;
        cube_transform.col(3).head<3>() = cube_pos;
        
        // Get rotation for the cube
        Vec3 cube_z = root_pos - cube_pos;
        cube_z.normalize();
        Vec3 cube_y = Vec3(0, 1, 0).cross(cube_z);
        Vec3 cube_x = cube_y.cross(cube_z);
        cube_transform.col(0).head<3>() = cube_x;
        cube_transform.col(1).head<3>() = cube_y;
        cube_transform.col(2).head<3>() = cube_z;


        /* Project root onto floor plane */
        // Set position of cube to floor
        Vec3 proj_pos = root_pos;
        proj_pos.y() = 0.1f;    // Cube on top of plane
        proj_transform.col(3).head<3>() = proj_pos;

        // Set rotation of cube
        Vec3 proj_y = Vec3(0, 1, 0);
        Vec3 proj_z = root_rot * Vec3(0, 0, 1);
        proj_z.y() = 0.0f;
        proj_z.normalize();
        Vec3 proj_x = proj_y.cross(proj_z);

        // Set values to transformation matrix
        proj_transform.col(0).head<3>() = proj_x;
        proj_transform.col(1).head<3>() = proj_y;
        proj_transform.col(2).head<3>() = proj_z;

    }

    void render() override
    {
        agl::Render::plane()
            ->scale(15.0f)
            ->color(0.15f, 0.15f, 0.15f)
            ->floor_grid(true)
            ->draw();

        agl::Render::model(model)->draw();

        agl::Render::cube()
            ->scale(0.5f, 0.5f, 0.5f)
            ->transform(cube_transform)
            ->color(1.0f, 0.0f, 0.0f)
            ->draw();

        agl::Render::cube()
            ->scale(0.2f, 0.2f, 0.2f)
            ->transform(proj_transform)
            ->color(0.0f, 1.0f, 0.0f)
            ->draw();
    }

};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
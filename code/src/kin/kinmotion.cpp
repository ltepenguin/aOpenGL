#include "aOpenGL/kin/kinmotion.h"
#include <thread>

namespace a::gl {

static std::vector<KinPose> _get_poses(const spKinModel& kmodel,
                                       const Motion&     motion)
{
    int pidx_n = motion.poses.size();
    Mat4 root_preTrf = kmodel->pre_trfs.at(0);

    // create poses
    std::vector<KinPose> poses;
    poses.reserve(pidx_n);
    for(int i = 0; i < pidx_n; ++i)
    {
        KinPose pose;

        // world_pos = basisTrf * local_pos
        Vec4 world_root_pos;
        world_root_pos.head<3>() = motion.poses.at(i).root_position;
        world_root_pos.w() = 1.0f;

        for(int j = 0; j < (int)kmodel->gl_jnt_idxes.size(); ++j)
        {
            int jidx = kmodel->gl_jnt_idxes.at(j);
            pose.local_rots.push_back(
                to_mat4(motion.poses.at(i).local_rotations.at(jidx))
            );
        }
        
        pose.world_basisTrf = Mat4::Identity();
        pose.root_preR  = root_preTrf;
        pose.local_pos  = world_root_pos;
        pose.world_trfs = kin::compute_fk(kmodel, pose.world_basisTrf, pose.local_pos, pose.local_rots);

        // this will compute root's local_pos and local_rots
        kin::reset_world_basisTrf(pose, pose.world_trfs.at(0));

        poses.push_back(std::move(pose));
    }
    
    return poses;
}

static void _get_poses_job(
    std::vector<std::vector<KinPose>>& out_poses,
    const std::vector<int>&            assigned_mids,
    const spKinModel&                  kmodel,
    const std::vector<Motion>&         motions)
{
    for(int mid : assigned_mids)
    {
        out_poses.at(mid) = _get_poses(kmodel, motions.at(mid));
    }
}

spKinMotion kinmotion(const spKinModel& kmodel,
                      const std::vector<Motion>& motions)
{
    auto kmotion = std::make_shared<KinMotion>();
    int nom = motions.size();

    // pose 관련 정보 설정
    std::vector<std::vector<KinPose>> vec_poses(nom);

    // read poses (parallel)
    {
        int thread_n = std::thread::hardware_concurrency();
        thread_n = std::min(thread_n, nom);
        
        // assign jobs
        std::vector<std::vector<int>> assigned_mids(nom);
        for(int i = 0; i < nom; ++i)
        {
            int thread_id = i % thread_n;
            assigned_mids.at(thread_id).push_back(i);
        }

        // do jobs
        std::vector<std::thread> threads;
        threads.reserve(thread_n);

        for(int i = 0; i < thread_n; ++i)
        {
            threads.push_back(std::thread(_get_poses_job, 
                std::ref(vec_poses),
                std::cref(assigned_mids.at(i)), 
                std::cref(kmodel), 
                std::cref(motions))
                );
        }

        for(auto& thread : threads)
            thread.join();
    }

    // set and return
    kmotion->kmodel = kmodel;
    for(int i = 0; i < nom; ++i)
    {
        const auto& poses_i = vec_poses.at(i);
        std::vector<int> mid_i(poses_i.size(), i);

        kmotion->start_pidxes.push_back((int)kmotion->poses.size());
        kmotion->poses.insert(
            std::end(kmotion->poses), std::begin(poses_i), std::end(poses_i)
            );
        kmotion->motion_ids.insert(
            std::end(kmotion->motion_ids), std::begin(mid_i), std::end(mid_i)
            );

        kmotion->motion_names.push_back(motions.at(i).name);
        kmotion->fbx_start_times.push_back(motions.at(i).start_time);
        kmotion->fbx_end_times.push_back(motions.at(i).end_time);
    }
    kmotion->motion_n = nom;
    
    return kmotion;
}

spKinMotion kinmotion(const spKinModel& kmodel, const std::vector<std::vector<Motion>>& motions_vec)
{
    int vec_size = motions_vec.size();
    std::vector<Motion> motions;
    motions.reserve(vec_size);
    for(int i = 0; i < (int)motions_vec.size(); ++i)
    {
        const std::vector<Motion>& motion_i = motions_vec.at(i);
        motions.insert(std::end(motions), std::begin(motion_i), std::end(motion_i));
    }
    return kinmotion(kmodel, motions);
}

bool KinMotion::is_same_motion(int pidx0, int pidx1)
{
    return kin::is_same_motion(shared_from_this(), pidx0, pidx1);
}

void KinMotion::apply_basisTrf_filter(int filter_size)
{
    spKinMotion kmotion = shared_from_this();
    return kin::apply_basisTrf_filter(kmotion, filter_size);
}

void KinMotion::init_world_basisTrf_from_shoulders(const std::string& Rshl, const std::string& Lshl)
{
    spKinMotion kmotion = shared_from_this();
    return kin::init_world_basisTrf_from_shoulders(kmotion, Rshl, Lshl);
}

int KinMotion::get_pidx(const std::string& take_name, int fbx_frame, int fbx_fps)
{
    spKinMotion kmotion = shared_from_this();
    return kin::get_pidx(kmotion, take_name, fbx_frame, fbx_fps);
}

int KinMotion::get_pidx(int mid, float time)
{
    spKinMotion kmotion = shared_from_this();
    return kin::get_pidx(kmotion, mid, time);
}

namespace kin {

// check if pidx0 and pidx1 is same motion
bool is_same_motion(const spKinMotion& kmotion, int pidx0, int pidx1)
{
    int pidx_n = kmotion->motion_ids.size();
    assert(pidx0 < pidx_n);
    assert(pidx1 < pidx_n);
    if((pidx0 < 0) || (pidx1 < 0))
        return false;
    return (kmotion->motion_ids.at(pidx0) == kmotion->motion_ids.at(pidx1));
}

// filter's window size: filter_size * 2 + 1
void apply_basisTrf_filter(spKinMotion& kmotion, int filter_size)
{
    // smooth basis orientations
    auto& poses = kmotion->poses;
    int pidx_n = poses.size();

    // smooth
    std::vector<Vec3> basisZs;
    std::vector<Vec3> basisTs;
    basisZs.reserve(pidx_n);
    basisTs.reserve(pidx_n);
    for(int pidx = 0; pidx < pidx_n; ++pidx)
    {
        int cnt = 0;
        Vec4 v4 = Vec4::Zero();
        Vec4 T4 = Vec4::Zero();
        
        // upper
        for(int j = 0; j <= filter_size; ++j)
        {
            if((pidx + j) >= pidx_n)
                break;
            if(is_same_motion(kmotion, pidx, pidx + j) == false)
                continue;
            
            v4 += poses.at(pidx + j).world_basisTrf.col(2);
            T4 += poses.at(pidx + j).world_basisTrf.col(3);
            cnt++;
        }

        // lower
        for(int j = 1; j <= filter_size; ++j)
        {
            if((pidx - j) < 0)
                break;
            if(is_same_motion(kmotion, pidx, pidx - j) == false)
                continue;
            
            v4 += poses.at(pidx - j).world_basisTrf.col(2);
            T4 += poses.at(pidx - j).world_basisTrf.col(3);
            cnt++;
        }
        assert(cnt > 0);

        v4 = v4 / cnt;
        T4 = T4 / cnt;
        basisZs.push_back(Vec3(v4.x(), v4.y(), v4.z()));
        basisTs.push_back(Vec3(T4.x(), T4.y(), T4.z()));
    }

    // recompute basis orient
    for(int i = 0; i < pidx_n; ++i)
    {
        Vec3 basisX = Vec3::UnitY().cross(basisZs.at(i));

        auto& pose = poses.at(i);
        pose.world_basisTrf.block<3, 1>(0, 0) = basisX.normalized();
        pose.world_basisTrf.block<3, 1>(0, 1) = Vec3::UnitY();
        pose.world_basisTrf.block<3, 1>(0, 2) = basisZs.at(i).normalized();
        pose.world_basisTrf.block<3, 1>(0, 3) = basisTs.at(i);
    }
}

void init_world_basisTrf_from_shoulders(spKinMotion& self, const std::string& Rshl, const std::string& Lshl)
{
    // find right & left shoulder index
    int Ridx = self->kmodel->jnt_name_to_idx.at(Rshl);
    int Lidx = self->kmodel->jnt_name_to_idx.at(Lshl);

    // set basis
    for(KinPose& pose : self->poses)
    {
        init_world_basisTrf_from_shoulders(pose, Ridx, Lidx);
    }
}
// get pidx
int get_pidx(const spKinMotion& kmotion, const std::string& take_name, int fbx_frame, int fbx_fps)
{
    int motion_n = kmotion->motion_names.size();
    int mid = -1;
    for(int i = 0; i < motion_n; ++i)
    {
        if(kmotion->motion_names.at(i) == take_name)
        {
            mid = i;
            break;
        }
    }
    assert(mid >= 0);    
    return get_pidx(kmotion, mid, (float)fbx_frame / (float)fbx_fps);
}

int get_pidx(const spKinMotion& kmotion, int mid, float time)
{
    int start_pidx = kmotion->start_pidxes.at(mid);

    float start_time = kmotion->fbx_start_times.at(mid);
    float end_time = kmotion->fbx_end_times.at(mid);
    assert(time >= start_time);
    assert(time < end_time);

    return start_pidx + std::round(60.0f * (time - start_time));
}

}
}
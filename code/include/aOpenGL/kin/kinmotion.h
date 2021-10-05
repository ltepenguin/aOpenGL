#pragma once
#include "kinpose.h"
#include "kinmodel.h"

namespace a::gl {

/**
 * @brief KinMotion의 joint 정보들과 KinModel의 joint 정보들과 같음
 * 
 */
struct KinMotion : public std::enable_shared_from_this<KinMotion>
{
    spKinModel                 kmodel;            // pointer

    // pose 관련 정보
    std::vector<KinPose>       poses;             // pidx = pose index
    std::vector<int>           motion_ids;        // 각 pidx가 속한 motion id
    
    // motion 관련 정보
    std::vector<int>           start_pidxes;      // 각 motion들의 시작 pidx
    int                        motion_n;          // number of motions
    
    // motion 추가 정보들 (optional)
    std::vector<std::string>   motion_names;      // 각 motion들의 name
    std::vector<float>         fbx_start_times;   // 각 motion들의 fbx에서 시작
    std::vector<float>         fbx_end_times;     // 각 motion들의 fbx에서 시작

    // functions
    bool is_same_motion(int pidx0, int pidx1);
    void apply_basisTrf_filter(int filter_size = 3);
    void init_world_basisTrf_from_shoulders(const std::string& Lshl, const std::string& Rshl);
    int  get_pidx(const std::string& take_name, int fbx_frame, int fbx_fps);
    int  get_pidx(int mid, float time);
};
using spKinMotion = std::shared_ptr<KinMotion>;

/**
 * @brief Constructor
 */
spKinMotion kinmotion(const spKinModel& kmodel, const std::vector<Motion>& motions);
spKinMotion kinmotion(const spKinModel& kmodel, const std::vector<std::vector<Motion>>& motions_vec);

namespace kin {

// check if pidx0 and pidx1 is same motion
bool is_same_motion(const spKinMotion& kmotion, int pidx0, int pidx1);

// filter's window size: filter_size * 2 + 1
void apply_basisTrf_filter(spKinMotion& kmotion, int filter_size = 3);

// set baseTrf using shoulder joints
void init_world_basisTrf_from_shoulders(spKinMotion& self, const std::string& Lshl, const std::string& Rshl);

// get pidx
int get_pidx(const spKinMotion& kmotion, const std::string& take_name, int fbx_frame, int fbx_fps);
int get_pidx(const spKinMotion& kmotion, int mid, float time);

}
}
#pragma once
#include "kinpose.h"
#include "kinmodel.h"

namespace a::gl {

struct KinMotion
{
    // model 관련 정보
    std::vector<std::string>   motion_jnt_names;
    std::map<std::string, int> jnt_name_to_motion_jnt_idx;
    std::vector<int>           kmodel_jnt_idxes;
    
    // pose 관련 정보
    std::vector<KinPose>       poses;             // pidx = pose index
    std::vector<int>           motion_ids;        // 각 pidx가 속한 motion id
    
    // motion 관련 정보
    std::vector<int>           start_pidxes;      // 각 motion들의 시작 pidx
    
    // motion 추가 정보들 (optional)
    std::vector<std::string>   motion_names;      // 각 motion들의 name
    std::vector<float>         fbx_start_times;   // 각 motion들의 fbx에서 시작
    std::vector<float>         fbx_end_times;     // 각 motion들의 fbx에서 시작
};
using spKinMotion = std::shared_ptr<KinMotion>;

}
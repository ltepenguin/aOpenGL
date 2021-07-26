#pragma once
#include "../model.h"

namespace a::gl {

/**
 * @brief   Kinematic Model
 *          OpenGL model을 건들지 않고 정보를 뽑을 수 있다.
 *          OpenGL model의 모든 joint 정보가 포함되지 않을 수 있다.
 *          0번째 joint는 항상 root joint.
 *          pre_transformation = local_translation * pre-rotation.
 */
struct KinModel : public std::enable_shared_from_this<KinModel>
{
    int                         noj;            // number of joints
    std::vector<std::string>    jnt_names;      // jnt_names
    std::vector<Mat4>           pre_trfs;       // jnt_names order.
    std::vector<int>            parent_idxes;   // jnt_names order.
    std::vector<int>            fk_order;       // fk computation order, 각 값은 joint index
    std::map<std::string, int>  jnt_name_to_idx;
    std::vector<int>            gl_jnt_idxes;   // agl::model에서 joint의 index

    /**
     * @brief           Forward Kinematics
     * 
     * @param T0        root position
     * @param local_Rs  local joint orientations in jnt_names order
     * @return          world joint transformations in jnt_names order
     */
    std::vector<Mat4> compute_fk(const Vec3& T0, const std::vector<Quat>& local_Rs);
    std::vector<Mat4> compute_fk(const Vec3& T0, const std::vector<Mat4>& local_Rs);
};
using spKinModel = std::shared_ptr<KinModel>;

// Constructors ---------------------------------------------------- //

/**
 * @brief           Create shared pointer of KinModel. AGL 모델의 모든 joint 들을 포함시킴.
 * @param model     aOpenGL 모델 정보
 */
spKinModel kinmodel(const spModel& model);

/**
 * @brief           Create shared pointer of KinModel. 
 * @param model     AGL 모델 정보
 * @param jnt_names kinmodel에 사용할 joint 정보들. 
 *                  0번째 joint는 반드시 root. 
 *                  fk에 필요한 joint들은 꼭 필요.
 *                  kinmodel은 jnt_names에 해당하는 joint 들만 포함.
 */
spKinModel kinmodel(const spModel& model, const std::vector<std::string>& jnt_names);

// Functions ------------------------------------------------------- //

namespace kin {

/**
 * @brief           Forward Kinematics
 * 
 * @param kmodel    kinmodel
 * @param world_T0  world root position
 * @param local_Rs  local joint rotations in jnt_names order. size: noj
 * @return          world trfs in jnt_names order
 */
std::vector<Mat4> compute_fk(const spKinModel&        kmodel,
                             const Vec3&              world_T0,
                             const std::vector<Quat>& local_Rs);

std::vector<Mat4> compute_fk(const spKinModel&        kmodel,
                             const Vec3&              world_T0,
                             const std::vector<Mat4>& local_Rs);

}
}
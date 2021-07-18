#pragma once
#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace a::gl {

/**
 * @brief 
 * TODO: support interpolation types
 */
enum class KeyInterpType { kUnknown, kConstant, kLinear, kCubic };

/**
 * @brief key frame
 */
struct KeyFrame
{
    float value{0.0};
    float time{0.0};
    KeyInterpType type{KeyInterpType::kUnknown};
};

/**
 * @brief 한 joint 혹은 node의 animation curve. 각 key 들은 시간 순서로 배치.
 */
struct NodeKeyFrames
{
    std::string name;
    glm::ivec3 euler_order;
    std::vector<KeyFrame> euler[3]; // degrees
    std::vector<KeyFrame> pos[3];
    std::vector<KeyFrame> scale[3];
};
using spNodeKeyFrames = std::shared_ptr<NodeKeyFrames>;

/**
 * @brief scene에 있는 모든 keyframe 집합
 */
struct SceneKeyFrames
{
    std::string name;
    std::vector<spNodeKeyFrames> node_keyframes;
    float start_time; // sec
    float end_time; // sec
};
using spSceneKeyFrames = std::shared_ptr<SceneKeyFrames>;

namespace keyframe {

/**
 * @brief resample single channel
 */
std::vector<KeyFrame> resample(const std::vector<KeyFrame>& frames, const std::vector<float>& timestep);

/**
 * @brief resample node/joint animation
 */
spNodeKeyFrames resample(spNodeKeyFrames, const std::vector<float>&);

/**
 * @brief resample all animations in scene
 */
spSceneKeyFrames resample(spSceneKeyFrames, const std::vector<float>&);

/**
 * @return rotations in names order. { names x nof }
 */
std::vector<std::vector<glm::quat>> get_rotations_from_resampled(const std::vector<std::string>& names, spSceneKeyFrames scene, int nof);

/**
 * @brief get trasnslation values
 */
//std::vector<glm::vec3> get_translations_from_resampled(std::string name, spSceneKeyFrames scene, int nof);
std::vector<glm::vec3> get_translations_from_resampled(std::string name, spSceneKeyFrames scene);

}

}
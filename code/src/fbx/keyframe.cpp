#include "keyframe.h"
#include <cmath>
#include <iostream>
#include <glm/gtx/quaternion.hpp>

namespace a::gl {

namespace keyframe {

static bool is_equal_time(float t0, float t1)
{
    if(std::abs(t0 - t1) < 0.0001)
        return true;
    else
        return false;
}

/**
 * @return target_time 보다 크지 않은 최대 frame의 index를 리턴.
 */
static int search_key(const std::vector<KeyFrame>& frames, float target_time, int iter_idx, bool upper_checked)
{
    if(iter_idx >= frames.size())
        return frames.size() - 1;
    if(iter_idx < 0)
        return 0;

    float iter_time = frames.at(iter_idx).time;
    
    if(is_equal_time(iter_time, target_time))
    {
        return iter_idx;
    }
    else if (iter_time > target_time)
    {
        return search_key(frames, target_time, iter_idx - 1, true);
    }
    else
    {
        if(upper_checked)
        {
            return iter_idx;
        }
        else
            return search_key(frames, target_time, iter_idx + 1, false);
    }    
}

static float interpolate_linear(float v0, float v1, float t, float t0, float t1)
{
    if(is_equal_time(t0, t1))
        return v0;
    else if(t >= t1)
        return v1;
    else if(t <= t0)
        return v0;
    
    float w0 = (t1 - t) / (t1 - t0);   
    return w0 * v0 + (1.0 - w0) * v1;
}

#if 0
#else

std::vector<KeyFrame> resample(const std::vector<KeyFrame>& frames, const std::vector<float>& timestep)
{
    std::vector<KeyFrame> new_keys;
    if(frames.size() == 0)
    {
        return new_keys;
    }
    new_keys.reserve(timestep.size());    
    
    int max_idx = frames.size() - 1;

    int lower_idx = 0;
    for(int i = 0; i < timestep.size(); ++i)
    {
        float ti = timestep.at(i);
        lower_idx = search_key(frames, ti, lower_idx, false);
        int upper_idx = std::min(lower_idx + 1, max_idx);
        
        // if(i == 5)
        // {
        //     if(frames.at(upper_idx).type == KeyInterpType::kLinear)
        //         std::cout << "linear" << std::endl;
        //     else if (frames.at(upper_idx).type == KeyInterpType::kUnknown)
        //         std::cout << "unknown" << std::endl;
        //     else if (frames.at(upper_idx).type == KeyInterpType::kConstant)
        //         std::cout << "constant" << std::endl;
        //     else if (frames.at(upper_idx).type == KeyInterpType::kCubic)
        //         std::cout << "cubic" << std::endl;
        // }
        
        //assert(frames.at(upper_idx).type == KeyInterpType::kLinear);
        //assert(frames.at(lower_idx).type == KeyInterpType::kLinear);

        // TODO: other interpolation type
        float v_t = interpolate_linear(frames.at(lower_idx).value,
                                       frames.at(upper_idx).value,
                                       ti,
                                       frames.at(lower_idx).time,
                                       frames.at(upper_idx).time);

        KeyFrame new_key;
        new_key.time = ti;
        new_key.value = v_t;
        new_key.type = frames.at(lower_idx).type;
        new_keys.push_back(new_key);
    }
    
    return new_keys;
}

spNodeKeyFrames resample(spNodeKeyFrames original, const std::vector<float>& timestep)
{
    auto resampled = std::make_shared<NodeKeyFrames>();   
    resampled->name = original->name;
    resampled->euler_order = original->euler_order;
    
    for(int i = 0; i < 3; ++i)
    {
        resampled->euler[i] = ::a::gl::keyframe::resample(original->euler[i], timestep);
        resampled->pos[i]   = ::a::gl::keyframe::resample(original->pos[i],   timestep);
        resampled->scale[i] = ::a::gl::keyframe::resample(original->scale[i], timestep);

        // std::cout << "original size: " << original->euler[i].size() 
        //     << " --> resampled size: " << resampled->euler[i].size() << std::endl;

    }
    return resampled;
}

spSceneKeyFrames resample(spSceneKeyFrames scene, const std::vector<float>& timestep)
{
    auto resampled = std::make_shared<SceneKeyFrames>();
    resampled->start_time = scene->start_time;
    resampled->end_time   = scene->end_time;
    resampled->name       = scene->name;
    
    int n = scene->node_keyframes.size();
    resampled->node_keyframes.reserve(n);
    
    for(int i = 0; i < n; ++i)
    {
        spNodeKeyFrames keys = ::a::gl::keyframe::resample(scene->node_keyframes.at(i), timestep);
        resampled->node_keyframes.push_back(keys);
    }
    return resampled;
}

#endif

std::vector<glm::quat> to_quaternions(const std::vector<float>& e0,
                                      const std::vector<float>& e1,
                                      const std::vector<float>& e2,
                                      const std::vector<glm::vec3>& axes)
{
    int n = e0.size();
    std::vector<glm::quat> quats;
    quats.reserve(n);

    for(int i = 0; i < n; ++i)
    {
        auto q0 = glm::angleAxis(e0.at(i), axes[0]);
        auto q1 = glm::angleAxis(e1.at(i), axes[1]);
        auto q2 = glm::angleAxis(e2.at(i), axes[2]);
        glm::quat q = q2 * q1 * q0;
        q = glm::normalize(q);
        quats.push_back(q);
    }

    return quats;
}

#if 0
#else

static std::vector<float> get_radians(const std::vector<KeyFrame>& keys, int nof)
{
    static float to_rad = M_PI / 180.0f;
    std::vector<float> angles;

    if(keys.size() == 0)
    {
        angles.resize(nof, 0);
        return angles;
    }
    else
    {
        assert(keys.size() == nof);
        angles.reserve(nof);
        for(int i = 0; i < nof; ++i)
        {
            angles.push_back(to_rad * keys.at(i).value);
        }
        return angles;
    }
}

static std::vector<float> get_values(const std::vector<KeyFrame>& keys)
{
    std::vector<float> values;
    
    if(keys.size() == 0)
    {
        return values;
    }
    else
    {
        int nof = keys.size();
        values.reserve(nof);
        for(int i = 0; i < nof; ++i)
        {
            values.push_back(keys.at(i).value);
        }
        return values;
    }
    
}

std::vector<std::vector<glm::quat>> get_rotations_from_resampled(const std::vector<std::string>& names, spSceneKeyFrames scene, int nof)
{
    std::vector<std::vector<glm::quat>> scene_animation;
    scene_animation.reserve(names.size());

    // name to node index
    std::map<std::string, int> name_to_nodeidx;
    for(int i = 0; i < scene->node_keyframes.size(); ++i)
    {
        name_to_nodeidx[scene->node_keyframes.at(i)->name] = i;
    }

    // iterate
    for(int i = 0; i < names.size(); ++i)
    {
        auto iter = name_to_nodeidx.find(names.at(i));
        if(iter == name_to_nodeidx.end())
        {
            // ! be careful with quat constructor order: w, x, y, z
            std::vector<glm::quat> rotations(nof, glm::quat(1.0, 0.0, 0.0, 0.0)); 
            scene_animation.push_back(rotations);
            std::cout << __FILE__ << "(" << __LINE__ << "): " << names.at(i) << " not found." << std::endl;
        }
        else
        {
            int idx = iter->second;
            spNodeKeyFrames node = scene->node_keyframes.at(idx);
            
            glm::ivec3 order = node->euler_order;
            
            std::vector<float> e0 = get_radians(node->euler[order.x], nof);
            std::vector<float> e1 = get_radians(node->euler[order.y], nof);
            std::vector<float> e2 = get_radians(node->euler[order.z], nof);

            // if(i == 0)
            // {
            //     printf("%d, %d, %d\n", order.x, order.y, order.z);
            //     for(int k = 0; k < e0.size(); ++k)
            //     {
            //         printf("%d: %.3g, %.3g, %.3g\n", k, e0[k], e1[k], e2[k]);
            //     }
            //     printf("\n");
            // }
            
            static const std::vector<glm::vec3> axes = {
                glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)
            };

            std::vector<glm::vec3> e_axes = { 
                axes[order.x], axes[order.y], axes[order.z]
            };
            
            auto rotations = to_quaternions(e0, e1, e2, e_axes);
            scene_animation.push_back(rotations);
        }
    }
    return scene_animation;
}
#endif

std::vector<glm::vec3> get_translations_from_resampled(std::string name, spSceneKeyFrames scene)
{
    // get index
    int idx = -1;
    for(int i = 0; i < scene->node_keyframes.size(); ++i)
    {
        auto name_i = scene->node_keyframes.at(i)->name;
        if(name_i == name)
        {
            idx = i;
            break;
        }
    }

    // iterate
    std::vector<glm::vec3> translations;
    if(idx == -1)
    {
        std::cout << __FILE__ << "(" << __LINE__ << "): " << name << " not found." << std::endl;
    }
    else
    {
        spNodeKeyFrames node = scene->node_keyframes.at(idx);
        auto kx = get_values(node->pos[0]);
        auto ky = get_values(node->pos[1]);
        auto kz = get_values(node->pos[2]);

        int xn = kx.size();
        int yn = ky.size();
        int zn = kz.size();

        int nof = std::max(std::max(xn, yn), zn);
        translations.reserve(nof);

        assert((xn == 0) || (xn == nof));
        assert((yn == 0) || (yn == nof));
        assert((zn == 0) || (zn == nof));

        for(int i = 0; i < nof; ++i)
        {
            float tx = (xn > 0) ? kx.at(i) : 0;
            float ty = (yn > 0) ? ky.at(i) : 0;
            float tz = (zn > 0) ? kz.at(i) : 0;
            
            translations.push_back(glm::vec3(tx, ty, tz));
            //translations.push_back(glm::vec3(kx.at(i), ky.at(i), kz.at(i)));
        }
    }
    return translations;
}

}
}

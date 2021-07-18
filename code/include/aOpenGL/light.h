#pragma once
#pragma GCC system_header
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace a::gl {

/**
 * @brief simple directional light
 * @author ckm
 */
struct Light
{
    glm::vec3 focus{0, 0, 0};
    glm::vec3 direction{glm::normalize(glm::vec3(0.5f, 1.5f, 1.0f))};
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
    float L{10.0f};

    glm::mat4 light_space_matrix() const;
};

}
#include "aOpenGL/light.h"

namespace a::gl {

glm::mat4 Light::light_space_matrix() const
{
    glm::vec3 lightPos = 10.0f * glm::normalize(direction) + focus;
    float near_plane = 1.0f, far_plane = 40.0f;
    
    // ortho: left, right, bottom, top
    glm::mat4 lightProjection = glm::ortho(-L, L, -L, L, near_plane, far_plane);
    
    // pos, center, upvec
    glm::mat4 lightView = glm::lookAt(lightPos, focus, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}

}
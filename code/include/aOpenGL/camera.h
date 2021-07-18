#pragma once
#pragma GCC system_header
#include <glm/glm.hpp>

namespace a::gl {

/**
 * @brief simple camera class
 */
class Camera
{
public:
    Camera();
    ~Camera();

	void mouse_input(float xoffset, float yoffset);	
    void mouse_scroll_input(float yoffset);
    void mouse_move(float xoffset, float yoffset);   

    void set_position(const glm::vec3& p);
    void set_focus(const glm::vec3& fp);
    void set_focus(const glm::vec3& fp, float distance);
    
    glm::mat4 view_matrix() const;
    glm::vec3 focus_position() const;
    glm::vec3 position() const;
    glm::vec3 direction() const;
    glm::mat3 orientation() const;
    float zoom() const { return 45.0f; }

private:
    void rotate(float yaw, float pitch);
    void update();

    glm::vec3 m_position;
    glm::mat3 m_orientation;
    glm::vec3 m_focus_position;
};

}
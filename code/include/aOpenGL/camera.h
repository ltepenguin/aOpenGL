#pragma once
#pragma GCC system_header
#include <glm/glm.hpp>
#include "eigentype.h"

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

    void set_position(const Vec3& p);
    void set_focus(const Vec3& fp);
    void set_focus(const Vec3& fp, float distance);

    void set_position(const glm::vec3& p);
    void set_focus(const glm::vec3& fp);
    void set_focus(const glm::vec3& fp, float distance);

    Mat4 view_matrix() const;
    Vec3 focus_position() const;
    Vec3 position() const;
    Vec3 direction() const;
    Mat3 orientation() const;
    
    glm::mat4 view_matrix_gl() const;
    glm::vec3 focus_position_gl() const;
    glm::vec3 position_gl() const;
    glm::vec3 direction_gl() const;
    glm::mat3 orientation_gl() const;
    
    float zoom() const { return 45.0f; }
    float ortho_zoom() const { return m_ortho_zoom; }
    
    float set_ortho_zoom(float val) { m_ortho_zoom = val; }
    void set_perspective(bool set_persp) { m_is_perspective = set_persp; }
    bool is_perspective() const { return m_is_perspective; }

private:
    void rotate(float yaw, float pitch);
    void update();

    glm::vec3 m_position;
    glm::mat3 m_orientation;
    glm::vec3 m_focus_position;
    bool m_is_perspective;
    float m_ortho_zoom;
};

}
#include "aOpenGL/camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace a::gl {

#define CAM_SENSITIVITY 0.0015f
static const glm::vec3 UP_AXIS(glm::vec3(0, 1, 0));

#if 0
static glm::mat3 getMat3(glm::vec3 xAxis, glm::vec3 yAxis, glm::vec3 zAxis)
{
    glm::mat3 rMat;
    
    rMat[0][0] = xAxis.x;
    rMat[0][1] = xAxis.y;
    rMat[0][2] = xAxis.z;

    rMat[1][0] = yAxis.x;
    rMat[1][1] = yAxis.y;
    rMat[1][2] = yAxis.z;

    rMat[2][0] = zAxis.x;
    rMat[2][1] = zAxis.y;
    rMat[2][2] = zAxis.z;
    
    return rMat;
}
glm::vec3 getAxis(const glm::mat3& mat, int idx)
{
    return glm::vec3(mat[idx][0], mat[idx][1], mat[idx][2]);
}
#endif

Camera::Camera(): m_position(glm::vec3(0, 1, 1)),
                  m_orientation(glm::mat3(1.0)),
                  m_focus_position(glm::vec3(0, 1, 0))
{
    this->update();
}

Camera::~Camera()
{}

void Camera::mouse_input(float xoffset, float yoffset)
{
    float rad   = 0.10f * glm::radians(-xoffset);
    float pitch = 0.10f * glm::radians(-yoffset);
    this->rotate(rad, pitch);
}

void Camera::mouse_scroll_input(float yoffset)
{
    glm::vec3 front = m_orientation[2];
    //glm::vec3 front = getAxis(m_orientation, 2);
    m_position += 0.1f * front * yoffset;
    m_focus_position += 0.1f * front * yoffset;
    this->update();
}

void Camera::mouse_move(float xoffset, float yoffset)
{
    xoffset *= CAM_SENSITIVITY;
    yoffset *= CAM_SENSITIVITY;
    
    glm::vec3 localXaxis = m_orientation[0];
    glm::vec3 localYaxis = m_orientation[1];
    //glm::vec3 localXaxis = getAxis(m_orientation, 0);
    //glm::vec3 localYaxis = getAxis(m_orientation, 1);
    glm::vec3 dT = xoffset * localXaxis + -yoffset * localYaxis;
    
    m_position += dT;
    m_focus_position += dT;
    this->update();
}

void Camera::set_position(const glm::vec3& p)
{
    m_position = p;
    update();
}

void Camera::set_focus(const glm::vec3& fp)
{
    m_focus_position = fp;
    update();
}

void Camera::set_focus(const glm::vec3& fp, float distance)
{
    m_focus_position = fp;
    glm::vec3 direction;
    // if(m_position.y < 2.5f)
    //     direction = glm::normalize(m_position - fp + glm::vec3(0, 0.01, 0));
    // else
    //     direction = glm::normalize(m_position - fp);
    direction = glm::normalize(m_position - fp);
    m_position = distance * direction + fp;
    update();
}

glm::mat4 Camera::view_matrix() const
{
    glm::vec3 up    = m_orientation[1];
    glm::vec3 front = m_orientation[2];
    //glm::vec3 up    = getAxis(m_orientation, 1);
    //glm::vec3 front = getAxis(m_orientation, 2);
    return glm::lookAt(m_position, m_position + front, up);
}

glm::vec3 Camera::focus_position() const
{
    return m_focus_position;
}

glm::vec3 Camera::position() const
{
    return m_position;
}

glm::vec3 Camera::direction() const
{
    return glm::normalize(m_position - m_focus_position);
}

glm::mat3 Camera::orientation() const
{
    return m_orientation;
}

void Camera::rotate(float yaw, float pitch)
{
    glm::vec3 v0 = m_position - m_focus_position;
    glm::vec3 pitchAxis = glm::cross(glm::normalize(v0), UP_AXIS);
    
    glm::quat yawR   = glm::angleAxis(yaw, UP_AXIS);
    glm::quat pitchR = glm::angleAxis(pitch, pitchAxis);    
    
    m_position = pitchR * yawR * v0 + m_focus_position;
    this->update();
}

void Camera::update()
{
    glm::vec3 z = glm::normalize(m_focus_position - m_position);
    glm::vec3 x = glm::cross(UP_AXIS, z);
    glm::vec3 y = glm::cross(z, x);
    
    //m_orientation = getMat3(x, y, z);
    m_orientation[0] = x;
    m_orientation[1] = y;
    m_orientation[2] = z;
}

}
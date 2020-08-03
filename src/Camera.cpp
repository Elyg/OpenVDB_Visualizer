#include "Camera.h"

Camera::Camera()
{
    //default constructor
}
Camera::Camera(glm::vec3 pos)
{
    m_pos = pos;
    m_handlePos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->computeViewMat();
}
void Camera::computeProjectionMat(float aspectRatio)
{
    m_projection = glm::perspective(m_fov, aspectRatio, m_near, m_far);
    //m_projection = glm::ortho(m_fov, aspectRatio, m_near, m_far);
}
void Camera::computeViewMat()
{
    m_view = glm::lookAt(m_pos, m_handlePos, m_up);
    //m_view = glm::inverse(m_view);
}

#include "Camera.h"

Camera::Camera()
{
  this->computeViewMat();
  this->computeProjectionMat();
}
Camera::Camera(glm::vec3 pos)
{
  m_pos = pos;
}
void Camera::computeProjectionMat()
{
  m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far);
  //m_projection = glm::ortho(m_fov, aspectRatio, m_near, m_far);
}
void Camera::computeViewMat()
{
  m_view = glm::lookAt(m_pos, m_handlePos, m_up);
}
void Camera::setWindowSize(uint32_t width, uint32_t height)
{
  m_aspectRatio = float(width)/float(height);
  computeProjectionMat();
}

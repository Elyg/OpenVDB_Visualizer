#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
  Camera();
  Camera(glm::vec3 pos);
  void computeProjectionMat(float aspectRatio);
  void computeViewMat();
  inline glm::mat4 getProjectionMat(){ return m_projection;}
  inline glm::mat4 getViewMat() {return m_view; }
  inline glm::vec3 getPos() {return m_pos;}
  inline float getFov() {return m_fov;}
  inline float getFocalLength() {return 1.0 / glm::tan(glm::pi<float>() / 180.0 * m_fov / 2.0);}
  
private:
  float m_fov = 45.0f;
  float m_near = 0.01f;
  float m_far = 1000.0f;
  
  glm::vec3 m_up;
  glm::vec3 m_pos;
  glm::vec3 m_handlePos;
  
  glm::mat4 m_projection;
  glm::mat4 m_view;
  
};
#endif

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
  Camera();
  Camera(glm::vec3 pos);
  void computeProjectionMat();
  void computeViewMat();
  
  void setWindowSize(uint32_t width, uint32_t height);
  
  inline glm::mat4 getProjectionMat(){ return m_projection;}
  inline glm::mat4 getViewMat() {return m_view; }
  inline glm::vec3 getPos() {return m_pos;}
  inline float getFov() {return m_fov;}
  inline float getFocalLength() {return 1.0f / glm::tan(glm::pi<float>() / 180.0f * m_fov / 2.0f);}

private:
  float m_fov = 45.0f;
  float m_near = 0.01f;
  float m_far = 1000.0f;

  float m_aspectRatio;

  glm::vec3 m_up = glm::vec3(0,1,0);
  glm::vec3 m_pos = glm::vec3(0,0,-0.3f);
  glm::vec3 m_handlePos = glm::vec3(0,0,0);

  glm::mat4 m_projection;
  glm::mat4 m_view;
  
};

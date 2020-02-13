#ifndef OpenGLWindow_H
#define OpenGLWindow_H

#include "VDB.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <QWidget>
#include <QOpenGLWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QKeyEvent>
#include <QMouseEvent>

#include <Camera.h>
#include <Shader.h>

#include <memory>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>



struct WinParams
{
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to store the x rotation mouse value
  //----------------------------------------------------------------------------------------------------------------------
  int spinXFace = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to store the y rotation mouse value
  //----------------------------------------------------------------------------------------------------------------------
  int spinYFace = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief flag to indicate if the mouse button is pressed when dragging
  //----------------------------------------------------------------------------------------------------------------------
  bool rotate = false;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief flag to indicate if the Right mouse button is pressed when dragging
  //----------------------------------------------------------------------------------------------------------------------
  bool translate = false;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the previous x mouse value
  //----------------------------------------------------------------------------------------------------------------------
  int origX = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the previous y mouse value
  //----------------------------------------------------------------------------------------------------------------------
  int origY = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the previous x mouse value for Position changes
  //----------------------------------------------------------------------------------------------------------------------
  int origXPos = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the previous y mouse value for Position changes
  //----------------------------------------------------------------------------------------------------------------------
  int origYPos = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief window width
  //----------------------------------------------------------------------------------------------------------------------
  int width = 1024;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief window height
  //----------------------------------------------------------------------------------------------------------------------
  int height = 720;
};

class OpenGLWindow : public QOpenGLWidget
{
  Q_OBJECT
public:
  OpenGLWindow();
  OpenGLWindow(QWidget *_parent);
  ~OpenGLWindow() override;
  
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int _w, int _h) override;
  void timerEvent(QTimerEvent *_event) override;
  void keyPressEvent(QKeyEvent *_event) override;
  void mouseMoveEvent(QMouseEvent *_event) override;
  void initFrameBuffer(GLuint texObj, GLuint texWidth, GLuint texHeight);
  GLuint initFace2DTex(GLuint bfTexWidth, GLuint bfTexHeight);
  void mousePressEvent(QMouseEvent *_event) override;
  void mouseReleaseEvent(QMouseEvent *_event) override;
  void wheelEvent(QWheelEvent *_event) override;
private:
  std::unique_ptr<Shader> m_shaderProgram;
  std::unique_ptr<Shader> m_shaderProgram2;
  std::unique_ptr<Shader> m_shaderProgram3;
  bool m_wireframe = false;
  unsigned int m_VAO;
  unsigned int m_VBO;
  unsigned int m_texture1;
  Camera *m_camera;
  glm::mat4 m_projection;
  glm::mat4 m_view;
  glm::mat4 m_model;
  
  glm::vec3 m_mousePos;
  
  WinParams m_win;
  int m_pointCount;
  GLuint m_textureId;
  GLuint m_frameBuffer;
  GLuint m_bfTexObj;
  
};

#endif // OpenGLWindow_H

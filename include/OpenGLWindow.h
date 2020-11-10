#pragma once

#include "glad/glad.h"

#include <QWidget>
#include <QOpenGLWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QKeyEvent>
#include <QMouseEvent>
//#include <QtWidgets>

#include <memory>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric> 

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/common.hpp> 
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>

#include "Camera.h"
#include "Shader.h"
#include "QColorRampEditor.h"
#include "OpenGLWindowParms.h"
#include "VDBPrim.h"


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
  void mousePressEvent(QMouseEvent *_event) override;
  void mouseReleaseEvent(QMouseEvent *_event) override;
  void wheelEvent(QWheelEvent *_event) override;
  
  void getIntersectionPoints(std::vector<std::vector<glm::vec3>> &array, glm::mat4 view, int slices);
  

  void setRampWidget(QColorRampEditor *widget);
  void updateVDB(std::string path);
  
public slots:
  void fitMinValuesChanged(double min);
  void fitMaxValuesChanged(double max);
  void vdbPathChanged(QString path);
  void slicesChanged(int slices);
  void densityValuesChanged(double densityMulti);
  void rampValuesChanged(const std::vector<float> &_colors);
  void setRampColorTable();
  
private:
  
  std::unique_ptr<VDBPrim> m_vdb;
  std::unique_ptr<Camera> m_camera;
  std::unique_ptr<Shader> m_shaderProxy;
  std::unique_ptr<Shader> m_shaderVolume;
  std::unique_ptr<Shader> m_shaderBBox;
  glm::mat4 m_projection;
  glm::mat4 m_view;
  glm::mat4 m_model;
  glm::vec3 m_min;
  glm::vec3 m_max;
  uint32_t m_VAO;
  uint32_t m_VBO;

  glm::vec3 m_mousePos;
  OpenGLWindowParms m_win;
 
  bool m_wireframe = false;
  int m_maxDim;
  int m_slices;
  float m_densityMulti;
  float m_rampMin;
  float m_rampMax;
  
  std::vector<float> m_ctable;
  QColorRampEditor *m_colorWidget;
  GLuint m_transferTextureId;
};

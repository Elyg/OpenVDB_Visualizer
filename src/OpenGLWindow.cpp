#include "OpenGLWindow.h"

OpenGLWindow::OpenGLWindow()
{
  // some default values for the visualizer
  m_slices = pow(2, 8);
  m_wireframe = false;
  m_rampMin = 0;
  m_rampMax = 1;
  m_densityMulti = 0.1f;
}

OpenGLWindow::OpenGLWindow(QWidget *_parent) : QOpenGLWidget( _parent)
{
  this->resize(_parent->size());
  m_slices = pow(2, 8);
  m_wireframe = false;
  m_rampMin = 0;
  m_rampMax = 1;
  m_densityMulti = 0.1f;
}

OpenGLWindow::~OpenGLWindow()
{
  //glDeleteVertexArrays(1, &m_VAO);
  //glDeleteBuffers(1, &m_VBO);
  std::cout<<"Cleaned VAO and VBO"<<std::endl;
}

void OpenGLWindow::setRampWidget(QColorRampEditor *widget)
{
  m_colorWidget = widget;
  setRampColorTable();
}

void OpenGLWindow::rampValuesChanged(std::vector<float> const &_colors)
{
  std::cout<<"Ramp changed"<<std::endl;
  m_ctable = _colors;  
}

void OpenGLWindow::fitMinValuesChanged(double min)
{
  m_rampMin = float(min);//.toFloat();
  std::cout<<"Min: "<<m_rampMin<<std::endl;
}
void OpenGLWindow::fitMaxValuesChanged(double max)
{
  m_rampMax = float(max);//.toFloat();
  std::cout<<"Max: "<<m_rampMax<<std::endl;
}
void OpenGLWindow::slicesChanged(int slices)
{
  m_slices = slices;//.toFloat();
  std::cout<<"Slices: "<<m_slices<<std::endl;
}

void OpenGLWindow::densityValuesChanged(double densityMulti)
{
  m_densityMulti = densityMulti;
  std::cout<<"Density Multi: x"<<m_densityMulti<<std::endl;
}

void OpenGLWindow::vdbPathChanged(QString path)
{
  std::ifstream ifile(path.toStdString());
  if(ifile)
  {
    updateVDB(path.toStdString());
    std::cout<<"New path: "<<path.toStdString()<<std::endl;
  }
  else
  {
    std::cout<<"File does not exist"<<std::endl;
  }
}

void OpenGLWindow::setRampColorTable()
{
  m_ctable = m_colorWidget->getColorTable();
  std::cout<<"Color Ramp Changed"<<std::endl;
}

void OpenGLWindow::updateVDB(std::string path)
{
  std::cout<<"Updating VDBs and textures..."<<std::endl;
  m_vdb = std::make_unique<VDBPrim>(path, "density");
  m_vdb->bindVDBTexture();
  m_shaderVolume->use();
  m_shaderVolume->setInt("volume", 0);
  
  glm::mat4 scale = glm::scale(m_vdb->getVoxelDim());
  m_model = scale*m_vdb->getTransform();
  std::cout << "Scale Mat: "<<glm::to_string(m_model) << std::endl;
  
}

void OpenGLWindow::initializeGL()
{
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  gladLoadGL();
  setFocus();

  // createShaders
  m_shaderProxy = std::make_unique<Shader>("../../shaders/volumeProxy.vert", "../../shaders/volumeProxy.frag");
  m_shaderVolume = std::make_unique<Shader>("../../shaders/volume.vert", "../../shaders/volume.frag");
  m_shaderBBox = std::make_unique<Shader>("../../shaders/bbox.vert", "../../shaders/bbox.frag", "../../shaders/bbox.geo");
    
  std::cout<<"Initializing VDBs and textures..."<<std::endl;
  std::string path = "../../data/pig2.vdb"; //default vdb
  m_vdb = std::make_unique<VDBPrim>(path, "density");
  m_vdb->bindVDBTexture();
  m_maxDim = m_vdb->getMaxDim();
  m_shaderVolume->use();
  m_shaderVolume->setInt("volume", 0);
  
  std::cout<<"Initializing Color ramp textures..."<<std::endl;
  //load data into a 3D texture
  glGenTextures(1, &m_transferTextureId);
  glBindTexture(GL_TEXTURE_1D, m_transferTextureId);
  
  // set the ramp texture parameters
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage1D(GL_TEXTURE_1D,0,GL_RGB32F,256,0,GL_RGB,GL_FLOAT, m_ctable.data());
  m_shaderVolume->setInt("transfer", 1);
  
  std::cout<<"Initializing Camera..."<<std::endl;
  //create default Camera
  m_camera = std::make_unique<Camera>();
  //set openGL window size
  m_camera->setWindowSize(m_win.width, m_win.height);
  
  //get projection, view, model matrices
  m_projection = m_camera->getProjectionMat();
  m_view = m_camera->getViewMat();
  
  glm::mat4 scale = glm::scale(m_vdb->getVoxelDim());
  std::cout << "Dim Size: "<<glm::to_string(m_vdb->getVoxelDim()) << std::endl;
  m_model = m_vdb->getTransform()*scale;
  std::cout << "Scale Mat: "<<glm::to_string(m_model) << std::endl;
  
  //initialize bounding min, max points
  
  std::vector<glm::vec3> bboxPoints 
  {                                                                                                                                                                                                                           
    glm::vec3(-0.5f, -0.5f, -0.5f), //min
    glm::vec3(0.5f, 0.5f, 0.5f),    //max
  };
  
  for (auto &point : bboxPoints)
  {
    point = glm::mat3(m_model)*point;
  }

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glBindVertexArray(m_VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, bboxPoints.size() * sizeof(glm::vec3), bboxPoints.data(), GL_STATIC_DRAW);
  
  GLuint posAttrib = glGetAttribLocation(m_shaderBBox->getId(), "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  
  startTimer(60);
  
}

void OpenGLWindow::resizeGL(int _w, int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  
  m_camera->setWindowSize(m_win.width, m_win.height);
  
}

void OpenGLWindow::paintGL()
{
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glViewport(0, 0, m_win.width, m_win.height);
  glClearColor(0.3f,0.3f,0.3f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glm::mat4 mouseRotX = glm::mat4(1);
  glm::mat4 mouseRotY = glm::mat4(1);
  mouseRotX = glm::rotate(mouseRotX, glm::radians(float(m_win.spinXFace)*0.2f), glm::vec3(1,0,0));
  mouseRotY = glm::rotate(mouseRotY, glm::radians(float(m_win.spinYFace)*0.2f), glm::vec3(0,1,0));
  glm::mat4 m_mouseTm = (mouseRotX*mouseRotY);
  glm::mat4 camPosTm = glm::translate(glm::mat4(1), m_mousePos+m_camera->getPos());
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, m_vdb->getTextureId());
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, m_transferTextureId);
  
  //setRampColorTable();
  glTexImage1D(GL_TEXTURE_1D,0,GL_RGB32F,256,0,GL_RGB,GL_FLOAT, m_ctable.data());

  if(m_wireframe == false)
  {
    m_shaderVolume->use();
    m_shaderVolume->setMat4("projection", m_projection);
    m_shaderVolume->setMat4("view", camPosTm*m_view*m_mouseTm);
    m_shaderVolume->setMat4("model", m_model);
    
    m_shaderVolume->setFloat("slices",float(m_slices));
    m_shaderVolume->setFloat("maxDim",float(m_maxDim));
    m_shaderVolume->setFloat("min",float(m_rampMin));
    m_shaderVolume->setFloat("max",float(m_rampMax));
    m_shaderVolume->setFloat("densityMulti",float(m_densityMulti));
  }
  else
  {
    m_shaderProxy->use();
    m_shaderProxy->setMat4("projection", m_projection);
    m_shaderProxy->setMat4("view", camPosTm*m_view*m_mouseTm);
    m_shaderProxy->setMat4("model", m_model);

    m_shaderProxy->setFloat("min",float(m_rampMin));
    m_shaderProxy->setFloat("max",float(m_rampMax));
  }

  std::vector<std::vector<glm::vec3>> intersectionSlices;
  m_vdb->createProxySlices(intersectionSlices, camPosTm*m_view*glm::inverse(m_vdb->getRotMat())*m_mouseTm, m_slices);
  int seed = 0;
  for(auto &intersectionPoints : intersectionSlices)
  {
   seed++;
   glm::vec3 cd = glm::ballRand(1.0);
   cd = glm::vec3(seed)/glm::vec3(m_slices);
   std::srand(seed*5);
   
   uint32_t VAO;
   uint32_t VBO;
   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);
   glGenBuffers(1, &VBO);
   glBindVertexArray(VAO);
   
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, intersectionPoints.size() * sizeof(glm::vec3), intersectionPoints.data(), GL_DYNAMIC_DRAW);

   if(m_wireframe == false)
   {
     m_shaderVolume->setVec3("color", glm::vec3(cd));
     
     GLuint posAttrib = glGetAttribLocation(m_shaderVolume->getId(), "position");
     glEnableVertexAttribArray(posAttrib);
     glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
   }
   else
   {
     m_shaderProxy->setVec3("color", glm::vec3(cd));
     
     GLuint posAttrib = glGetAttribLocation(m_shaderProxy->getId(), "position");
     glEnableVertexAttribArray(posAttrib);
     glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
   }
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, intersectionPoints.size());
  }
  //DRAW BBOX
  m_shaderBBox->use();
  
  m_shaderBBox->setMat4("projection", m_projection);
  m_shaderBBox->setMat4("view", camPosTm*m_view*m_mouseTm);
  m_shaderBBox->setMat4("model", m_model);
  
  m_shaderBBox->setVec3("min", glm::vec3(-1.0f));
  m_shaderBBox->setVec3("max", glm::vec3(1.0f));
  
  m_shaderBBox->setVec3("color", glm::vec3(1.0f, 0.5f, 0.0f));
  glEnable(GL_LINE_SMOOTH);
  glBindVertexArray(m_VAO);
  glDrawArrays(GL_LINES, 0, 2);
}

void OpenGLWindow::keyPressEvent(QKeyEvent *_event)
{
  switch(_event->key())
    {
    case Qt::Key_W :
      m_wireframe = true;
      std::cout<<'W'<<std::endl; break;
    case Qt::Key_C :
      m_wireframe = false;
      std::cout<<'C'<<std::endl; break;
    case Qt::Key_Space :
      m_mousePos = glm::vec3(0);
      m_win.spinXFace = 0;
      m_win.spinYFace = 0;
      m_wireframe = false;
      std::cout<<"SPACE"<<std::endl; break;
    default: break;
    }
  update();
}

void OpenGLWindow::timerEvent(QTimerEvent *_event)
{
  update();
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWindow::mouseMoveEvent( QMouseEvent* _event )
{
  if(m_win.rotate == true)
    {
      int diffX = _event->x() - m_win.origX;
      int diffY = _event->y() - m_win.origY;
      m_win.spinXFace -= diffY;
      m_win.spinYFace += diffX;
      m_win.origX = _event->x();
      m_win.origY = _event->y();
      update();
    }
  if(m_win.translate == true)
    {
      int diffX = _event->x() - m_win.origXPos;
      int diffY = _event->y() - m_win.origYPos;
      m_mousePos.x += diffX*0.005;
      m_mousePos.y -= diffY*0.005;
      m_win.origXPos = _event->x();
      m_win.origYPos = _event->y();
      update();
    }
}


//----------------------------------------------------------------------------------------------------------------------
void OpenGLWindow::mousePressEvent( QMouseEvent* _event )
{
  if(_event->button() == Qt::LeftButton)
    {
      m_win.rotate = true;
      m_win.origX = _event->x();
      m_win.origY = _event->y();
      //std::cout<<"Left Pressed"<<std::endl;
      update();
    }
  if(_event->button() == Qt::RightButton)
    {
      m_win.translate = true;
      m_win.origXPos = _event->x();
      m_win.origYPos = _event->y();
      //std::cout<<"Right Pressed"<<std::endl;
      update();
    }
}

//----------------------------------------------------------------------------------------------------------------------
void OpenGLWindow::mouseReleaseEvent( QMouseEvent* _event )
{
  if(_event->button() == Qt::LeftButton)
    {
      m_win.rotate = false;
      //std::cout<<"Left Released"<<std::endl;
      update();
    }
  if(_event->button() == Qt::RightButton)
    {
      m_win.translate = false;
      //std::cout<<"Right Released"<<std::endl;
      update();
    }
}

//----------------------------------------------------------------------------------------------------------------------
void OpenGLWindow::wheelEvent( QWheelEvent* _event )
{
  float speed = 2;
  if ( _event->delta() > 0 )
    {
      m_mousePos.z -= 0.1f*speed;
    }
  if ( _event->delta() < 0 )
    {
      m_mousePos.z += 0.1f*speed;
    }
}

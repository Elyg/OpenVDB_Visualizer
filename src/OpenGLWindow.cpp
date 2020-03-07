#include "OpenGLWindow.h"


OpenGLWindow::OpenGLWindow()
{}

OpenGLWindow::OpenGLWindow(QWidget *_parent) : QOpenGLWidget( _parent)
{
  this->resize(_parent->size());
}

OpenGLWindow::~OpenGLWindow()
{
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
  std::cout<<"Cleaned VAO and VBO"<<std::endl;
}

// init the 2D texture for render backface 'bf' stands for backface
GLuint OpenGLWindow::initFace2DTex(GLuint bfTexWidth, GLuint bfTexHeight)
{
  GLuint backFace2DTex;
  glGenTextures(1, &backFace2DTex);
  glBindTexture(GL_TEXTURE_2D, backFace2DTex);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bfTexWidth, bfTexHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
  return backFace2DTex;
}
// init the framebuffer, the only framebuffer used in this program
void OpenGLWindow::initFrameBuffer(GLuint texObj, GLuint texWidth, GLuint texHeight)
{
  // create a depth buffer for our framebuffer
  GLuint depthBuffer;
  glGenRenderbuffers(1, &depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);
  //glRenderbufferStorageMultisample( GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, texWidth, texWidth);
  //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, texWidth, texHeight);
  // attach the texture and the depth buffer to the framebuffer
  glGenFramebuffers(1, &m_frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
  
  GLenum complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (complete != GL_FRAMEBUFFER_COMPLETE)
    {
      std::cout << "framebuffer is not complete" << std::endl;
      exit(EXIT_FAILURE);
    }
  std::cout << m_frameBuffer << std::endl;
  glEnable(GL_DEPTH_TEST);    
}
//// init the framebuffer, the only framebuffer used in this program
//void OpenGLWindow::initFrameBuffer(GLuint texObj, GLuint texWidth, GLuint texHeight)
//{
//  // create a depth buffer for our framebuffer
//  GLuint depthBuffer;
//  glGenRenderbuffers(1, &depthBuffer);
//  glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
//  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);
//  //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, texWidth, texHeight);
//  // attach the texture and the depth buffer to the framebuffer
//  glGenFramebuffers(1, &m_frameBuffer);
//  glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
//  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj, 0);
//  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
  
//  GLenum complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//  if (complete != GL_FRAMEBUFFER_COMPLETE)
//    {
//      std::cout << "framebuffer is not complete" << std::endl;
//      exit(EXIT_FAILURE);
//    }
//  std::cout << m_frameBuffer << std::endl;
//  glEnable(GL_DEPTH_TEST);    
//}
void OpenGLWindow::initializeGL()
{
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  gladLoadGL();
  
  setFocus();
  glEnable( GL_DEPTH_TEST );
  glEnable(GL_MULTISAMPLE);
  
  // need to reformat this
  m_shaderProgram.reset(new Shader("../../shaders/shader.vert", "../../shaders/shader.frag"));
  m_shaderProgram2.reset(new Shader("../../shaders/shaderSimple.vert", "../../shaders/shaderSimple.frag"));
  m_shaderProgram3.reset(new Shader("../../shaders/backface.vert", "../../shaders/backface.frag"));
  
  // loding in a vdb grid
  std::string path = "../../data/cloud.vdb";
  std::shared_ptr<VDB> vdb = std::make_shared<VDB>(path);

  
  GLfloat vertices[24] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 1.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 1.0
  };
  
  
  GLuint elements[36] = {
    1,5,7,
    7,3,1,
    0,2,6,
    6,4,0,
    0,1,3,
    3,2,0,
    7,5,4,
    4,6,7,
    2,3,7,
    7,6,2,
    1,0,4,
    4,5,1
  };
  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(m_VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);

  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
  
  GLuint posAttrib = glGetAttribLocation(m_shaderProgram->getId(), "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  
  posAttrib = glGetAttribLocation(m_shaderProgram2->getId(), "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  
  posAttrib = glGetAttribLocation(m_shaderProgram3->getId(), "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  int XDIM = vdb->getDenseGrid()->bbox().dim()[0];
  int YDIM = vdb->getDenseGrid()->bbox().dim()[1];
  int ZDIM = vdb->getDenseGrid()->bbox().dim()[2];
  
  std::cout << XDIM <<std::endl;
  std::cout << YDIM <<std::endl;
  std::cout << ZDIM <<std::endl;
  
  
  //load data into a 3D texture
  glGenTextures(1, &m_textureId);
  glBindTexture(GL_TEXTURE_3D, m_textureId);
  
  // set the texture parameters GL_CLAMP_TO_BORDER
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glTexImage3D(GL_TEXTURE_3D,0,GL_R32F,XDIM,YDIM,ZDIM,0,GL_RED,GL_FLOAT,vdb->getDenseGrid()->data());
  
  // camera
  m_camera = new Camera(glm::vec3(0.0f, 0.0f, -0.3f));
  float aspectRatio = (float(m_win.width)/float(m_win.height));
  m_camera->computeProjectionMat(aspectRatio);
  m_projection = m_camera->getProjectionMat();
  m_view = m_camera->getViewMat();
  m_model = glm::scale(glm::mat4( 1.0f ), vdb->getScale());

  std::cout << glm::to_string(m_model) << std::endl;
  
  m_shaderProgram->use();
  m_shaderProgram->setInt("volume", m_textureId);
  m_shaderProgram->setVec2("ScreenSize", glm::vec2((float)m_win.width, (float)m_win.height));

  GLint qt_buffer;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &qt_buffer);
  std::cout << qt_buffer << std::endl;
  //glActiveTexture(GL_TEXTURE2);
  m_bfTexObj = this->initFace2DTex(m_win.width, m_win.height);
  //glBindTexture(GL_TEXTURE_2D, 0);
  std::cout << m_bfTexObj << std::endl;
  m_shaderProgram->setInt("ExitPoints", m_bfTexObj);
  this->initFrameBuffer(m_bfTexObj, m_win.width, m_win.width);
  startTimer(60);

}

void OpenGLWindow::resizeGL(int _w, int _h)
{

  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  float aspectRatio = (float(m_win.width)/float(m_win.height));
  m_camera->computeProjectionMat(aspectRatio);
  m_projection = m_camera->getProjectionMat();
  
  glDeleteTextures(1, &m_bfTexObj);
  glDeleteFramebuffers(1, &m_frameBuffer);
  m_bfTexObj = this->initFace2DTex(m_win.width, m_win.height);
  m_shaderProgram->setInt("ExitPoints", m_bfTexObj);
  this->initFrameBuffer(m_bfTexObj, m_win.width, m_win.width);
}


void OpenGLWindow::paintGL()
{
  //glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CCW);
  //glEnable(GL_TEXTURE_3D);
  glEnable(GL_BLEND);
  glEnable(GL_POLYGON_SMOOTH);
  //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
  //wcglBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glDisable(GL_MULTISAMPLE);
  //GLint qt_buffer;
  //glGetIntegerv(GL_FRAMEBUFFER_BINDING, &qt_buffer);
  glm::mat4 mouseRotX = glm::mat4(1);
  glm::mat4 mouseRotY = glm::mat4(1);
  mouseRotX = glm::rotate(mouseRotX, glm::radians(float(m_win.spinXFace)*0.2f), glm::vec3(1,0,0));
  mouseRotY = glm::rotate(mouseRotY, glm::radians(float(m_win.spinYFace)*0.2f), glm::vec3(0,1,0));
  glm::mat4 m_mouseTm = (mouseRotX*mouseRotY);
  glm::mat4 camPosTm = glm::translate(glm::mat4(1), m_mousePos+m_camera->getPos());
  
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBuffer);
  glDisable(GL_MULTISAMPLE);
  //glEnable(GL_POLYGON_SMOOTH);
  //glEnable(GL_MULTISAMPLE);
  glViewport(0, 0, m_win.width, m_win.height);
  glClearColor(0.2f,0.2f,0.2f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_bfTexObj);
  m_shaderProgram3->use();
  m_shaderProgram3->setMat4("projection", m_projection);
  m_shaderProgram3->setMat4("view", camPosTm*m_view*m_mouseTm);
  m_shaderProgram3->setMat4("model", m_model);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  glDisable(GL_CULL_FACE);
  
  //https://stackoverflow.com/questions/42878216/opengl-how-to-draw-to-a-multisample-framebuffer-and-then-use-the-result-as-a-n
  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
  //glDisable(GL_MULTISAMPLE);
  //glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glViewport(0, 0, m_win.width, m_win.height);
  glClearColor(0.2f,0.2f,0.2f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_3D, m_textureId);
  m_shaderProgram->use();
  m_shaderProgram->setMat4("projection", m_projection);
  m_shaderProgram->setMat4("view", camPosTm*m_view*m_mouseTm);
  //glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.9,0.9,0.9));
  m_shaderProgram->setMat4("model", m_model);
  m_shaderProgram->setInt("ExitPoints", m_bfTexObj);
  m_shaderProgram->setInt("volume", m_textureId);
  m_shaderProgram->setVec2("ScreenSize", glm::vec2((float)m_win.width, (float)m_win.height));
  //m_shaderProgram->setVec2("ScreenSize", glm::vec2(2048, 2048));
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  glDisable(GL_CULL_FACE);
  glUseProgram(0);
  
  if(m_wireframe == true)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
  else
    {
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
  
  
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
  float speed = 10;
  if ( _event->delta() > 0 )
    {
      m_mousePos.z -= 0.1f*speed;
    }
  if ( _event->delta() < 0 )
    {
      m_mousePos.z += 0.1f*speed;
    }
}

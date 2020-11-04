#include "OpenGLWindow.h"

void _print(glm::vec3 vec)
{
 std::cout << glm::to_string(vec) << std::endl;
}

template <typename T>
std::vector<size_t> sort_indexes(const std::vector<T> &v) {

  // initialize original index locations
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values 
  std::stable_sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}

void OpenGLWindow::getIntersectionPoints(std::vector<std::vector<glm::vec3>> &array, glm::mat4 view, int slices)
{
  std::vector<glm::vec3> points 
  {                                                                                                                                                                                                                           
    glm::vec3(0.0f, 0.0f, 0.0f), //0
    glm::vec3(0.0f, 0.0f, 1.0f), //1
    glm::vec3(0.0f, 1.0f, 0.0f), //2
    glm::vec3(0.0f, 1.0f, 1.0f), //3
    glm::vec3(1.0f, 0.0f, 0.0f), //4
    glm::vec3(1.0f, 0.0f, 1.0f), //5
    glm::vec3(1.0f, 1.0f, 0.0f), //6
    glm::vec3(1.0f, 1.0f, 1.0f)  //7
  };
  glm::mat3 camViewTM = glm::mat3(view);

  for (auto &point : points)
  {
    point += glm::vec3(-0.5f,-0.5f,-0.5f);
    point = camViewTM*point;
  }

  std::vector<std::vector<glm::vec3>> edges
  {
    std::vector<glm::vec3> {points[0], points[1]},
    std::vector<glm::vec3> {points[1], points[5]},
    std::vector<glm::vec3> {points[5], points[4]},
    std::vector<glm::vec3> {points[4], points[0]},

    std::vector<glm::vec3> {points[2], points[3]},
    std::vector<glm::vec3> {points[3], points[7]},
    std::vector<glm::vec3> {points[7], points[6]},
    std::vector<glm::vec3> {points[6], points[2]},

    std::vector<glm::vec3> {points[0], points[2]},
    std::vector<glm::vec3> {points[1], points[3]},
    std::vector<glm::vec3> {points[5], points[7]},
    std::vector<glm::vec3> {points[4], points[6]}
  }; 

  float minz = points[0][2];
  float maxz = 0;
  for (auto &point : points)
  {
    minz = glm::min(minz, point[2]);
    maxz = glm::max(maxz, point[2]);
  }

  float step = maxz-minz;
  step /= float(slices)+1;

  std::vector<std::vector<glm::vec3>> slicePoints;

  for(int i=0; i<slices; ++i)
  {
      
    glm::vec3 planeP = glm::vec3(0.0f, 0.0f , 0.0f);
    glm::vec3 planeN = glm::vec3(0.0f, 0.0f, -1.0f);

    float dt = step*float(i+1);

    planeP[2] += minz;
    planeP[2] += (maxz-minz)*dt;

    std::vector<glm::vec3> intersectPoints;

    for (auto &point : edges)
    {
      glm::vec3 rayP = point[0];
      glm::vec3 rayDir = glm::normalize(point[1]-point[0]);
      
      float ndotu = dot(rayDir, planeN);
      if(glm::abs(ndotu) > 0.0001f)
      {
        glm::vec3 diff = rayP - planeP;
        float prod1 = dot(diff, planeN);
        float prod2 = dot(rayDir, planeN);
        float prod3 = prod1/prod2;
        glm::vec3 p = rayP - rayDir*prod3;
        
        float l = glm::length(point[1]-point[0]);
        glm::vec3 pr = p-point[0];
        float proj = dot(pr, rayDir);
        proj = glm::clamp(proj, 0.0f, l);

        glm::vec3 cp = point[0] + proj*rayDir;
        if(glm::length(cp-p) < 0.0001f)
        {
          intersectPoints.push_back(p);
          //_print(p);
        }
      }
    }
    if(intersectPoints.size() > 0)
    {
      
      //slicePoints.push_back(intersectPoints);
      glm::vec3 avgPoint = glm::vec3(0.0f, 0.0f, 0.0f);
      for (auto &intersectPoint : intersectPoints)
      {
        avgPoint += intersectPoint;
      }
      avgPoint /= float(intersectPoints.size());
      
      //auto copyIntersectPoints = intersectPoints;
      std::vector<float> angles;
      for(auto copyIntersectPoint : intersectPoints)
      {
        copyIntersectPoint -= avgPoint;
        copyIntersectPoint[2] = 0.0f;
        float angle = std::atan2(copyIntersectPoint[1], copyIntersectPoint[0]);
        angles.push_back(angle);
      }

      std::vector<glm::vec3> sortedIntersectPoints (intersectPoints.size());
      int j = 0;
      for(auto &index : sort_indexes(angles))
      {
        sortedIntersectPoints[j] = intersectPoints[index];
        j++;
      }
      std::reverse(sortedIntersectPoints.begin(), sortedIntersectPoints.end());
      std::vector<glm::vec3> final;
      if(sortedIntersectPoints.size() > 3)
      {
        final.push_back(glm::inverse(camViewTM)*avgPoint);
        
        int f=0;
        for (auto &point : sortedIntersectPoints)
        {
          f++;
          final.push_back(glm::inverse(camViewTM)*point);
        }
        final.push_back(glm::inverse(camViewTM)*sortedIntersectPoints[0]);
      }
      else
      {
          for (auto point : sortedIntersectPoints)
          {
            final.push_back(glm::inverse(camViewTM)*point);
          }
      }
      slicePoints.push_back(final);
    } 
  }
  array = slicePoints;
}


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

void OpenGLWindow::setRampWidget(QColorRampEditor *widget)
{
  m_colorWidget = widget;
  setRampColorTable();
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
}

void OpenGLWindow::initVDB(std::string path)
{
  // loding in a vdb grid
  //std::string path = "../../data/pig2.vdb";
  std::shared_ptr<VDB> vdb = std::make_shared<VDB>(path);
  m_min = vdb->getMin();
  m_max = vdb->getMax();

  int XDIM = vdb->getDenseGrid()->bbox().dim()[0];
  int YDIM = vdb->getDenseGrid()->bbox().dim()[1];
  int ZDIM = vdb->getDenseGrid()->bbox().dim()[2];
  
  m_maxDim = std::max(XDIM,YDIM);
  m_maxDim = std::max(m_maxDim, ZDIM);
  
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
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage3D(GL_TEXTURE_3D,0,GL_R32F,XDIM,YDIM,ZDIM,0,GL_RED,GL_FLOAT,vdb->getDenseGrid()->data());
  m_shaderProgram2->use();
  m_shaderProgram2->setInt("volume", 0);
}

void OpenGLWindow::updateVDB(std::string path)
{
  
  std::shared_ptr<VDB> vdb = std::make_shared<VDB>(path);
  m_min = vdb->getMin();
  m_max = vdb->getMax();

  int XDIM = vdb->getDenseGrid()->bbox().dim()[0];
  int YDIM = vdb->getDenseGrid()->bbox().dim()[1];
  int ZDIM = vdb->getDenseGrid()->bbox().dim()[2];
  
  m_maxDim = std::max(XDIM,YDIM);
  m_maxDim = std::max(m_maxDim, ZDIM);
  
  std::cout << XDIM <<std::endl;
  std::cout << YDIM <<std::endl;
  std::cout << ZDIM <<std::endl;
  
  std::cout<<m_textureId<<std::endl;
  
  //glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, m_textureId);
  glTexImage3D(GL_TEXTURE_3D,0,GL_R32F,XDIM,YDIM,ZDIM,0,GL_RED,GL_FLOAT,vdb->getDenseGrid()->data());
  m_shaderProgram2->use();
  m_shaderProgram2->setInt("volume", 0);
  update();
  std::cout<<m_textureId<<std::endl;
}

void OpenGLWindow::initializeGL()
{
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  gladLoadGL();
  setFocus();
  
  // need to reformat this
  m_shaderProgram.reset(new Shader("../../shaders/debug.vert", "../../shaders/debug.frag"));
  m_shaderProgram2.reset(new Shader("../../shaders/shaderSimple.vert", "../../shaders/shaderSimple.frag"));
  
  initVDB("../../data/pig2.vdb");
  
  //load data into a 3D texture
  glGenTextures(1, &m_transferTextureId);
  glBindTexture(GL_TEXTURE_1D, m_transferTextureId);
  
  // set the texture parameters GL_CLAMP_TO_BORDER
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage1D(GL_TEXTURE_1D,0,GL_RGB32F,256,0,GL_RGB,GL_FLOAT, m_ctable.data());
  
  m_shaderProgram2->setInt("transfer", 1);
  
  // camera
  m_camera = new Camera(glm::vec3(0.0f, 0.0f, -0.3f));
  float aspectRatio = (float(m_win.width)/float(m_win.height));
  m_camera->computeProjectionMat(aspectRatio);
  m_projection = m_camera->getProjectionMat();
  m_view = m_camera->getViewMat();
  //m_model = glm::scale(glm::mat4( 1.0f ), vdb->getScale());
  m_model = glm::mat4(1.0f);
  std::cout << glm::to_string(m_model) << std::endl;
  
  m_pop = 0;
  m_slices = pow(2, 8);
  m_wireframe = false;
  m_rampMin = 0;
  m_rampMax = 1;
  m_densityMulti = 0.1f;
  startTimer(60);

}

void OpenGLWindow::resizeGL(int _w, int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
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
  glBindTexture(GL_TEXTURE_3D, m_textureId);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, m_transferTextureId);
  
  setRampColorTable();
  glTexImage1D(GL_TEXTURE_1D,0,GL_RGB32F,256,0,GL_RGB,GL_FLOAT, m_ctable.data());

  if(m_wireframe == false)
  {
    m_shaderProgram2->use();
    m_shaderProgram2->setMat4("projection", m_projection);
    m_shaderProgram2->setMat4("view", camPosTm*m_view*m_mouseTm);
    m_shaderProgram2->setMat4("model", m_model);
    m_shaderProgram2->setFloat("slices",float(m_slices));
    m_shaderProgram2->setFloat("maxDim",float(m_maxDim));
    m_shaderProgram2->setFloat("min",float(m_rampMin));
    m_shaderProgram2->setFloat("max",float(m_rampMax));
    m_shaderProgram2->setFloat("densityMulti",float(m_densityMulti));
  }
  else
  {
    m_shaderProgram->use();
    m_shaderProgram->setMat4("projection", m_projection);
    m_shaderProgram->setMat4("view", camPosTm*m_view*m_mouseTm);
    m_shaderProgram->setMat4("model", m_model);
    m_shaderProgram->setFloat("slices",float(m_slices));
    m_shaderProgram->setFloat("maxDim",float(m_maxDim));
    m_shaderProgram->setFloat("min",float(m_rampMin));
    m_shaderProgram->setFloat("max",float(m_rampMax));
  }

  std::vector<std::vector<glm::vec3>> intersectionSlices;
  getIntersectionPoints(intersectionSlices, camPosTm*m_view*m_mouseTm, m_slices);
  int seed = 0;
  for(auto &intersectionPoints : intersectionSlices)
  {
   seed++;
   glm::vec3 cd = glm::ballRand(1.0);
   cd = glm::vec3(seed)/glm::vec3(m_slices);
   std::srand(seed*5);
   if(m_wireframe == false)
   {
     m_shaderProgram2->setVec3("color", glm::vec3(cd));
     m_shaderProgram2->setFloat("slice", float(seed));
   }
   else
   {
     m_shaderProgram->setVec3("color", glm::vec3(cd));
     m_shaderProgram->setFloat("slice", float(seed));
   }

    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, intersectionPoints.size() * sizeof(glm::vec3), intersectionPoints.data(), GL_DYNAMIC_DRAW);
  
    GLuint posAttrib = glGetAttribLocation(m_shaderProgram2->getId(), "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    //glPointSize(3.0f);
    //glDrawArrays(GL_POINTS, 0, intersectionPoints.size());
    glDrawArrays(GL_TRIANGLE_FAN, 0, intersectionPoints.size());

  }
}

void OpenGLWindow::keyPressEvent(QKeyEvent *_event)
{
  switch(_event->key())
    {
    case Qt::Key_W :
      m_wireframe = true;
      m_pop += 1;
      std::cout<<'W'<<std::endl; break;
    case Qt::Key_C :
      m_wireframe = false;
      m_pop -= 1;
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

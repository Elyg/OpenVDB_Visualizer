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
  //camViewTM = glm::mat3(1.0f);
  //camViewTM = glm::rotate(glm::mat4(camViewTM), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  //camViewTM = glm::rotate(glm::mat4(camViewTM), glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
    
    //std::cout << "i: " << i << "delta: " << dt << std::endl;
    
    planeP[2] += minz;
    planeP[2] += (maxz-minz)*dt;

    std::vector<glm::vec3> intersectPoints;

    for (auto &point : edges)
    {
      //std::cout << glm::to_string(point[0]) << "," << glm::to_string(point[1]) << std::endl;
      glm::vec3 rayP = point[0];
      glm::vec3 rayDir = glm::normalize(point[1]-point[0]);
      //_print(rayDir);
      
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
      std::cout << intersectPoints.size() << std::endl;
      for(auto &pt : intersectPoints)
      {
        std::cout << "Unsorted Slice: " << i << " ";
        _print(pt);
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
      
      auto copyIntersectPoints = intersectPoints;
      std::vector<float> angles;
      for(auto &copyIntersectPoint : copyIntersectPoints)
      {
        
        copyIntersectPoint -= avgPoint;
        copyIntersectPoint[2] = 0.0f;
        float angle = std::atan2(copyIntersectPoint[1], copyIntersectPoint[0]);
        angles.push_back(angle);
      }

      std::vector<glm::vec3> sortedIntersectPoints = intersectPoints;
      int j = 0;
      auto indexes = sort_indexes(angles);
      for(auto &index : indexes)
      {
        sortedIntersectPoints[j] = intersectPoints[index];
        j++;
      }

      std::vector<glm::vec3> final;
      if(sortedIntersectPoints.size() > 3)
      {
        //final.push_back(glm::inverse(camViewTM)*avgPoint);
      
      int f=0;
      for (auto point : sortedIntersectPoints)
      {
        final.push_back(glm::inverse(camViewTM)*point);
        if(f%2 == 0)
        {
          final.push_back(glm::inverse(camViewTM)*avgPoint);
        }
        
        f++;
      }
      if(sortedIntersectPoints.size()%2==0)
        {
          final.push_back(glm::inverse(camViewTM)*avgPoint);
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
      if(sortedIntersectPoints.size() > 3)
      {
        //final.push_back(sortedIntersectPoints[1]);
      }
      std::cout << final.size() << std::endl;
      for(auto &pt : final)
      {
        std::cout << "Sorted Slice: " << i << " ";
        _print(pt);
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

void OpenGLWindow::initializeGL()
{
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  gladLoadGL();
  setFocus();
  
  // need to reformat this
  m_shaderProgram.reset(new Shader("../../shaders/shader.vert", "../../shaders/shader.frag"));
  m_shaderProgram2.reset(new Shader("../../shaders/shaderSimple.vert", "../../shaders/shaderSimple.frag"));
  //m_shaderProgram3.reset(new Shader("../../shaders/backface.vert", "../../shaders/backface.frag"));
  
  // loding in a vdb grid
  std::string path = "../../data/pig.vdb";
  std::shared_ptr<VDB> vdb = std::make_shared<VDB>(path);
  m_min = vdb->getMin();
  m_max = vdb->getMax();

//  GLfloat vertices[24] = {
//    0.0, 0.0, 0.0, //0
//    0.0, 0.0, 1.0, //1
//    0.0, 1.0, 0.0, //2
//    0.0, 1.0, 1.0, //3
//    1.0, 0.0, 0.0, //4
//    1.0, 0.0, 1.0, //5
//    1.0, 1.0, 0.0, //6
//    1.0, 1.0, 1.0  //7
//  };
  
  
//  GLuint elements[36] = {
//    1,5,7,
//    7,3,1,
//    0,2,6,
//    6,4,0,
//    0,1,3,
//    3,2,0,
//    7,5,4,
//    4,6,7,
//    2,3,7,
//    7,6,2,
//    1,0,4,
//    4,5,1
//  };
//  glGenVertexArrays(1, &m_VAO);
//  glGenBuffers(1, &m_VBO);
//  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
//  glBindVertexArray(m_VAO);
  
//  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//  GLuint ebo;
//  glGenBuffers(1, &ebo);

  
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
  
//  GLuint posAttrib = glGetAttribLocation(m_shaderProgram2->getId(), "position");
//  glEnableVertexAttribArray(posAttrib);
//  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  
  
  int XDIM = vdb->getDenseGrid()->bbox().dim()[0];
  int YDIM = vdb->getDenseGrid()->bbox().dim()[1];
  int ZDIM = vdb->getDenseGrid()->bbox().dim()[2];
  
  std::cout << XDIM <<std::endl;
  std::cout << YDIM <<std::endl;
  std::cout << ZDIM <<std::endl;

  //load data into a 3D texture
  glGenTextures(0, &m_textureId);
  glBindTexture(GL_TEXTURE_3D, m_textureId);
  
  // set the texture parameters GL_CLAMP_TO_BORDER
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage3D(GL_TEXTURE_3D,0,GL_R32F,XDIM,YDIM,ZDIM,0,GL_RED,GL_FLOAT,vdb->getDenseGrid()->data());
  glBindTexture(GL_TEXTURE_3D, 0);
  //glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  // camera
  m_camera = new Camera(glm::vec3(0.0f, 0.0f, -0.3f));
  float aspectRatio = (float(m_win.width)/float(m_win.height));
  m_camera->computeProjectionMat(aspectRatio);
  m_projection = m_camera->getProjectionMat();
  m_view = m_camera->getViewMat();
  //m_model = glm::scale(glm::mat4( 1.0f ), vdb->getScale());
  m_model = glm::mat4(1.0f);
  std::cout << glm::to_string(m_model) << std::endl;
  
  m_shaderProgram2->use();
  m_shaderProgram2->setInt("volume", m_textureId);
  
  //m_min = glm::vec3(0);
  //m_max = vdb->getScale();
  
  //std::cout << glm::to_string(vdb->getScale()) << std::endl;
  //std::cout << glm::to_string(vdb->getScale()) << std::endl;
  
  m_pop = 0;
  m_wireframe = false;
  startTimer(60);

}

void OpenGLWindow::resizeGL(int _w, int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  //float aspectRatio = (float(m_win.width)/float(m_win.height));
  
  //m_camera->computeProjectionMat(aspectRatio);
  //m_projection = m_camera->getProjectionMat();
  
  //m_viewportSize = glm::vec2(m_win.width, m_win.height);
  //m_aspectRatio = aspectRatio;
  
  //m_shaderProgram->use();
  //m_shaderProgram->setFloat("aspect_ratio", m_aspectRatio);
  //m_shaderProgram->setVec2("viewport_size", m_viewportSize);
}


void OpenGLWindow::paintGL()
{
  glEnable(GL_BLEND);
  //glEnable(GL_ALPHA_TEST);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
 //glEnable(GL_MULTISAMPLE);
  glViewport(0, 0, m_win.width, m_win.height);
  glClearColor(0.2f,0.2f,0.2f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, m_textureId);
  
  glm::mat4 mouseRotX = glm::mat4(1);
  glm::mat4 mouseRotY = glm::mat4(1);
  mouseRotX = glm::rotate(mouseRotX, glm::radians(float(m_win.spinXFace)*0.2f), glm::vec3(1,0,0));
  mouseRotY = glm::rotate(mouseRotY, glm::radians(float(m_win.spinYFace)*0.2f), glm::vec3(0,1,0));
  glm::mat4 m_mouseTm = (mouseRotX*mouseRotY);
  glm::mat4 camPosTm = glm::translate(glm::mat4(1), m_mousePos+m_camera->getPos());
  
  int slices = pow(2, 3); //9
  
  m_shaderProgram2->use();
  m_shaderProgram2->setMat4("projection", m_projection);
  m_shaderProgram2->setMat4("view", camPosTm*m_view*m_mouseTm);
  m_shaderProgram2->setMat4("model", m_model);
  
  m_shaderProgram->setFloat("slices",float(slices));
  //m_shaderProgram2->setMat4("projection", m_projection);
  //m_shaderProgram2->setMat4("view", camPosTm*m_view*m_mouseTm);
  //m_shaderProgram2->setMat4("model", m_model);
  
  //m_shaderProgram->setFloat("focal_length", m_camera->getFocalLength());
  //m_shaderProgram->setFloat("aspect_ratio", m_aspectRatio);
  //m_shaderProgram->setVec2("viewport_size", m_viewportSize);
  //m_shaderProgram->setVec3("ray_origin", m_mousePos+m_camera->getPos());
  //m_shaderProgram->setVec3("top", m_max);
  //m_shaderProgram->setVec3("bottom", m_min);
  
  //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  //glDisableVertexAttribArray(m_VAO);
  std::vector<std::vector<glm::vec3>> intersectionSlices;
  getIntersectionPoints(intersectionSlices, camPosTm*m_view*m_mouseTm, slices);
  int seed = 0;
  for(auto &intersectionPoints : intersectionSlices)
  {
   seed++;
   glm::vec3 cd = glm::ballRand(1.0);
   std::srand(seed*5);
   m_shaderProgram2->setVec3("color", glm::vec3(cd));
   
   for(int i=0; i<m_pop; ++i)
     {
       
       //intersectionPoints.pop_back();
     }
   
    //int sizeVertices = sizeof(intersectionPoints) / sizeof(glm::vec3);
    //std::cout << sizeVertices << std::endl;
    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, intersectionPoints.size() * sizeof(glm::vec3), intersectionPoints.data(), GL_STATIC_DRAW);
  
    GLuint posAttrib = glGetAttribLocation(m_shaderProgram2->getId(), "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    //glPointSize(3.0f);
    glDrawArrays(GL_POINTS, 0, intersectionPoints.size());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, intersectionPoints.size());
    //glDisableVertexAttribArray(VAO);
  }

   //glEnableVertexAttribArray(m_VAO);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

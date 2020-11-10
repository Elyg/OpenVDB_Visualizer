#include "VDBPrim.h"

VDBPrim::VDBPrim()
{
  openvdb::initialize();
}

VDBPrim::VDBPrim(const std::string &_file, const std::string &gridName)
{
  std::ifstream ifile(_file);
  if(ifile)
  {
    openvdb::initialize();
    m_filePath = _file;
    setGridNames();
    loadGrid(gridName);
    initAttributes();
  }
}

VDBPrim::~VDBPrim()
{
  glDeleteTextures(1, &m_textureId);
}

void VDBPrim::setGridNames()
{
  openvdb::io::File file(m_filePath);
  file.open();
  for (openvdb::io::File::NameIterator nameIter = file.beginName();
      nameIter != file.endName(); ++nameIter)
  {
    m_gridNames.push_back(nameIter.gridName());
  }
  file.close();
}

void VDBPrim::loadGrid(const std::string &gridName)
{
  openvdb::io::File file(m_filePath);
  file.open();

  auto vdb_grid = file.readGrid(gridName);
  openvdb::CoordBBox bbox = vdb_grid->evalActiveVoxelBoundingBox();
  openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(vdb_grid);
  m_voxelSize = glm::vec3(grid->voxelSize().x(), grid->voxelSize().y(), grid->voxelSize().z());
  auto layout = openvdb::tools::MemoryLayout::LayoutXYZ;
  m_denseGrid = std::make_unique<openvdb::tools::Dense<float, openvdb::tools::MemoryLayout::LayoutXYZ>> (bbox, layout);
  openvdb::tools::copyToDense(*grid, *m_denseGrid);
  vdb_grid->print();
  
  auto tm = vdb_grid->transformPtr()->baseMap()->getAffineMap()->getMat4();
  
  m_transform[0] = glm::vec4(tm.row(0)[0], tm.row(0)[1], tm.row(0)[2], tm.row(0)[3]);
  m_transform[1] = glm::vec4(tm.row(1)[0], tm.row(1)[1], tm.row(1)[2], tm.row(1)[3]);
  m_transform[2] = glm::vec4(tm.row(2)[0], tm.row(2)[1], tm.row(2)[2], tm.row(2)[3]);
  m_transform[3] = glm::vec4(tm.row(3)[0], tm.row(3)[1], tm.row(3)[2], tm.row(3)[3]);
  
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 translation;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(m_transform, scale, rotation, translation, skew,perspective);
  rotation = glm::conjugate(rotation);
  
  m_rot4 = glm::toMat4(rotation);
  
  openvdb::math::Extrema stats = openvdb::tools::extrema(grid->beginValueOn(), true);
  stats.print();
  m_denseGrid->print();
  std::cout<<"Transform Mat: "<<glm::to_string(m_transform)<<std::endl;
  std::cout<<"Rotation Mat: "<<glm::to_string(m_rot4)<<std::endl;
  file.close();
}
void VDBPrim::initAttributes()
{
  openvdb::CoordBBox bbox = m_denseGrid->bbox(); 
  openvdb::math::Vec3d center = m_denseGrid->bbox().getCenter();
  m_center = glm::vec3(center.x(), center.y(), center.z());
  m_max = glm::vec3(bbox.max().x(),bbox.max().y(),bbox.max().z() );
  m_min = glm::vec3(bbox.min().x(),bbox.min().y(),bbox.min().z() );
  m_scale =  m_max-m_min;
  m_scale *= m_voxelSize;

  bbox = m_denseGrid->bbox();
  center = bbox.getCenter();
  std::cout << center << std::endl;
  std::cout << bbox << std::endl;
  
  m_xdim = m_denseGrid->bbox().dim()[0];
  m_ydim = m_denseGrid->bbox().dim()[1];
  m_zdim = m_denseGrid->bbox().dim()[2];
  
  m_maxDim = std::max(m_xdim,m_ydim);
  m_maxDim = std::max(m_maxDim, m_zdim);
  
  m_voxelDim = glm::vec3(float(m_xdim), float(m_ydim), float(m_zdim));
  
  std::cout << m_xdim <<std::endl;
  std::cout << m_ydim <<std::endl;
  std::cout << m_zdim <<std::endl;
}
void VDBPrim::createSphere()
{
  //openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create(/*background value=*/0.0f);
  //openvdb::FloatGrid::Ptr grid = openvdb::tools::LevelSetSphere<openvdb::FloatGrid::Ptr>(1.0, openvdb::Vec3f(0.0f, 0.0f, 0.0f));
  //grid->setName("density");
}

void VDBPrim::bindVDBTexture()
{
  //gladLoadGL();
  //load data into a 3D texture
  glGenTextures(1, &m_textureId);
  glBindTexture(GL_TEXTURE_3D, m_textureId);
  
  // set the texture parameters GL_CLAMP_TO_BORDER
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage3D(GL_TEXTURE_3D,0,GL_R32F,m_xdim,m_ydim,m_zdim,0,GL_RED,GL_FLOAT,m_denseGrid->data());
  //m_shaderProgram2->use();
  //m_shaderProgram2->setInt("volume", 0);
}

void VDBPrim::updateVDBTexture()
{
  //glBindTexture(GL_TEXTURE_3D, m_textureId);
  //glTexImage3D(GL_TEXTURE_3D,0,GL_R32F,m_xdim,m_ydim,m_zdim,0,GL_RED,GL_FLOAT,m_denseGrid->data());
}

template <typename T> std::vector<size_t> VDBPrim::sort_indexes(const std::vector<T> &v) {

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

void VDBPrim::createProxySlices(std::vector<std::vector<glm::vec3>> &array, glm::mat4 view, int slices)
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

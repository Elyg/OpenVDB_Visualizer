#pragma once

#include <iostream>
#include <memory>
#include <fstream>

#include "glad/glad.h"

#ifndef Q_MOC_RUN
#    pragma push_macro("Q_FOREACH")
#    pragma push_macro("foreach")
#    pragma push_macro("slots")
#    undef Q_FOREACH
#    undef foreach
#    undef slots
#    include <openvdb/openvdb.h>
#    include <openvdb/tools/DenseSparseTools.h>
#    include <openvdb/tools/Dense.h>
#    include <openvdb/tools/Statistics.h>
#    include <openvdb/math/Stats.h>
#    pragma pop_macro("Q_FOREACH")
#    pragma pop_macro("foreach")
#    pragma pop_macro("slots")
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

class VDBPrim
{
public:
  
  VDBPrim();
  VDBPrim(const std::string &_file, const std::string &gridName="density");
  ~VDBPrim();
  
  void loadGrid(const std::string &gridName="density");
  void createSphere();
  void setGridNames();
  void bindVDBTexture();
  void updateVDBTexture();
  void initAttributes();
  void createProxySlices(std::vector<std::vector<glm::vec3>> &array, glm::mat4 view, int slices);
  
  template <typename T> std::vector<size_t> sort_indexes(const std::vector<T> &v);
  
  inline glm::mat4 const& getTransform(){return m_transform; }
  inline glm::mat4 const& getRotMat(){return m_rot4;}
  inline int getMaxDim(){return m_maxDim;}
  inline GLuint getTextureId() {return m_textureId; }
  inline std::vector<std::string> const& getGridNames() {return m_gridNames;}
  inline std::unique_ptr<openvdb::tools::Dense<float,  openvdb::tools::MemoryLayout::LayoutXYZ>> const& getDenseGrid() {return m_denseGrid;}
  inline glm::vec3 getScale(){return m_scale;}
  inline glm::vec3 getVoxelSize(){return m_voxelSize;}
  inline glm::vec3 getVoxelDim(){return m_voxelDim;}
  inline glm::vec3 getMin() { return m_min;}
  inline glm::vec3 getMax() { return m_max;}
  
private:
  //grid data
  std::unique_ptr<openvdb::tools::Dense<float,  openvdb::tools::MemoryLayout::LayoutXYZ>> m_denseGrid;
  glm::vec3 m_center;
  glm::vec3 m_min;
  glm::vec3 m_max;
  glm::vec3 m_scale;
  glm::vec3 m_voxelSize;
  glm::vec3 m_voxelDim;
  int m_maxDim;
  int m_xdim;
  int m_ydim;
  int m_zdim;
  
  glm::mat4 m_transform;
  glm::mat4 m_rot4;
  //proxy data
  
  
  //file data
  std::string m_filePath;
  std::vector<std::string> m_gridNames;
  
  //opengGL data
  GLuint m_textureId;
  
};

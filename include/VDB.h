#ifndef VDB_H
#define VDB_H

#include <iostream>
#pragma once
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

#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>


class VDB
{
public:
  VDB();
  VDB(std::string _file);
  ~VDB();
  void printData();
  void init();
  void getWorldSpaceVoxels(std::vector<float> &_vertices);
  inline std::shared_ptr<openvdb::tools::Dense<float,  openvdb::tools::MemoryLayout::LayoutXYZ>> getDenseGrid() {return m_denseGrid;}
  inline glm::vec3 getScale(){return m_scale;}
  inline glm::vec3 getMin() { return m_min;}
  inline glm::vec3 getMax() { return m_max;}
private:
  openvdb::GridBase::Ptr m_density;
  //openvdb::tools::Dense<float> *m_denseDensity;
  std::shared_ptr<openvdb::tools::Dense<float,  openvdb::tools::MemoryLayout::LayoutXYZ>> m_denseGrid;
  glm::vec3 m_center;
  glm::vec3 m_min;
  glm::vec3 m_max;
  glm::vec3 m_scale;
  glm::vec3 m_voxelSize;
  
};

#endif // VDB_H

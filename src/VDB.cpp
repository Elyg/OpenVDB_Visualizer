#include "VDB.h"

VDB::VDB()
{

}

VDB::~VDB()
{
    //delete m_denseDensity;
}

VDB::VDB(std::string _file)
{
    openvdb::initialize();
    openvdb::io::File file(_file);
    file.open();
    // only reading density gird for now
    m_density = file.readGrid("density");
    openvdb::CoordBBox bbox = m_density->evalActiveVoxelBoundingBox();
    openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(m_density);
    
    openvdb::math::Extrema stats = openvdb::tools::extrema(grid->beginValueOn(), true);
    stats.print();
    ///m_denseDensity = new openvdb::tools::Dense<float> (bbox);
    auto layout = openvdb::tools::MemoryLayout::LayoutXYZ;
    m_denseGrid = std::make_shared<openvdb::tools::Dense<float, openvdb::tools::MemoryLayout::LayoutXYZ>> (bbox, layout);
    openvdb::tools::copyToDense(*grid, *m_denseGrid);
    bbox = m_denseGrid->bbox();
    
    openvdb::math::Vec3d center = m_denseGrid->bbox().getCenter();
    m_voxelSize = glm::vec3(grid->voxelSize().x(), grid->voxelSize().y(), grid->voxelSize().z());
    m_center = glm::vec3(center.x(), center.y(), center.z());
    m_max = glm::vec3(bbox.max().x(),bbox.max().y(),bbox.max().z() );
    m_min = glm::vec3(bbox.min().x(),bbox.min().y(),bbox.min().z() );
    m_scale =  m_max- m_min;
    m_scale *= m_voxelSize;
    
    m_density->print();
    m_denseGrid->print();
    
    //m_transform = grid->transform();
    
    bbox = m_density->evalActiveVoxelBoundingBox();
    center = bbox.getCenter();
    std::cout << center << std::endl;
    std::cout << bbox << std::endl;
    auto m_metadata = file.readGridMetadata("density");

    file.close();


}

void VDB::printData()
{
    std::cout<<"VDB name: "<<m_density->getName()<<std::endl;
    for (auto iter = m_density->beginMeta(); iter != m_density->endMeta(); ++iter)
    {
        const std::string& name = iter->first;
        openvdb::Metadata::Ptr value = iter->second;
        std::string valueAsString = value->str();
        std::cout << name << " = " << valueAsString << std::endl;
    }

    openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(m_density);

    for (auto iter = grid->beginValueOn(); iter; ++iter)
    {
        std::cout << "Grid" << iter.getCoord() << " = " << grid->indexToWorld(iter.getCoord())
                  << " = " << *iter << std::endl;
    }

}

void VDB::getWorldSpaceVoxels(std::vector<float> &_vertices)
{
    openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(m_density);

    for (auto iter = grid->beginValueOn(); iter; ++iter)
    {
        auto P = grid->indexToWorld(iter.getCoord());
        _vertices.push_back(P.x());
        _vertices.push_back(P.y());
        _vertices.push_back(P.z());
    }

}

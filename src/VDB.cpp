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
    
    
    
    
    bbox = m_density->evalActiveVoxelBoundingBox();
    center = bbox.getCenter();
    std::cout << center << std::endl;
    std::cout << bbox << std::endl;
    auto m_metadata = file.readGridMetadata("density");
   //auto m_metadata = m_denseGrid->memoryLayout();
    //std::cout << m_metadata << std::endl;
    file.close();
    //openvdb::tools::copyToDense();
    //m_metadata = file.readGridMetadata("density");
//    for (openvdb::io::File::NameIterator nameIter = file.beginName();
//        nameIter != file.endName(); ++nameIter)
//    {
//        // Read in only the grid we are interested in.
//        if (nameIter.gridName() == "LevelSetSphere") {
//            baseGrid = file.readGrid(nameIter.gridName());
//        } else {
//            std::cout << "skipping grid " << nameIter.gridName() << std::endl;
//        }
//    }

}

void VDB::init()
{
    // Initialize the OpenVDB library.  This must be called at least
        // once per program and may safely be called multiple times.
        openvdb::initialize();
//        // Create an empty floating-point grid with background value 0.
//        openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create();
//        std::cout << "Testing random access:" << std::endl;
//        // Get an accessor for coordinate-based access to voxels.
//        openvdb::FloatGrid::Accessor accessor = grid->getAccessor();
//        // Define a coordinate with large signed indices.
//        openvdb::Coord xyz(1000, -200000000, 30000000);
//        // Set the voxel value at (1000, -200000000, 30000000) to 1.
//        accessor.setValue(xyz, 1.0);
//        // Verify that the voxel value at (1000, -200000000, 30000000) is 1.
//        std::cout << "Grid" << xyz << " = " << accessor.getValue(xyz) << std::endl;
//        // Reset the coordinates to those of a different voxel.
//        xyz.reset(1000, 200000000, -30000000);
//        // Verify that the voxel value at (1000, 200000000, -30000000) is
//        // the background value, 0.
//        std::cout << "Grid" << xyz << " = " << accessor.getValue(xyz) << std::endl;
//        // Set the voxel value at (1000, 200000000, -30000000) to 2.
//        accessor.setValue(xyz, 2.0);
//        // Set the voxels at the two extremes of the available coordinate space.
//        // For 32-bit signed coordinates these are (-2147483648, -2147483648, -2147483648)
//        // and (2147483647, 2147483647, 2147483647).
//        accessor.setValue(openvdb::Coord::min(), 3.0f);
//        accessor.setValue(openvdb::Coord::max(), 4.0f);
//        std::cout << "Testing sequential access:" << std::endl;
//        // Print all active ("on") voxels by means of an iterator.
//        for (openvdb::FloatGrid::ValueOnCIter iter = grid->cbeginValueOn(); iter; ++iter) {
//            std::cout << "Grid" << iter.getCoord() << " = " << *iter << std::endl;
//        }
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

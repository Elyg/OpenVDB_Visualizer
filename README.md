# OpenVDB_Visualizer (WIP)
This is my personal project for OpenVDB volume visualization with openGL, Qt5
Still work in progress. Need to fix a few bugs, clean the code, then start adding features like loading multiple grids, remapping values, ui etc.

<img src="https://github.com/Elyg/OpenVDB_Visualizer/blob/master/demo/vdb_visulizer.png" width="640">

# Library dependancies

 - OpenGL
 - OpenVDB
 - Qt5
 - cmake
 
### How to compile and run the application
```
git clone git@github.com:Elyg/OpenVDB_Visualizer.git
cd OpenVDB_Visualizer/
cmake CmakeLists.txt
make
./VDB_Visualizer
```

# Some References:

[https://www.openvdb.org/documentation/](https://www.openvdb.org/documentation/)

[https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-39-volume-rendering-techniques](https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-39-volume-rendering-techniques)

[https://en.wikipedia.org/wiki/Volume_ray_casting](https://en.wikipedia.org/wiki/Volume_ray_casting)

cmake_minimum_required(VERSION 3.10.2)

set(PROJECT_NAME VDB_VISUALIZER)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

#Bring the headers
include_directories(include ${PROJECT_SOURCE_DIR}/include
                            ${PROJECT_SOURCE_DIR}/include/glad
                            ${PROJECT_SOURCE_DIR}/include/KHR
                            )

file(GLOB SOURCES
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.ui"
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/shaders/*"
    "${PROJECT_SOURCE_DIR}/textures/*"
    "${PROJECT_SOURCE_DIR}/data/*"
    )

add_executable(${PROJECT_NAME} ${SOURCES})

list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB/")
find_package(OpenVDB REQUIRED)

find_package(glm)
find_package(Qt5 REQUIRED
    Core
    Gui
    Widgets
)

#ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
set(OPENVDB_ABI_VERSION_NUMBER 7)
target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui glm -ldl OpenVDB::openvdb)

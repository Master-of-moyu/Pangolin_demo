cmake_minimum_required(VERSION 3.11)
project(visualizeGeometry)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})
message(STATUS "Found Eigen in: " ${EIGEN3_INCLUDE_DIR})

find_package(Pangolin)
include_directories(${Pangolin_INCLUDE_DIRS})
message(STATUS "Found Pangolin in: " ${Pangolin_INCLUDE_DIRS})

add_executable(test
# hello.cpp
# visualizeGeometry.cpp
HelloPangolinOffscreen.cpp
)
target_link_libraries(test ${Pangolin_LIBRARIES})

add_subdirectory(SimpleDisplay)
add_subdirectory(SimpleDisplayImage)
add_subdirectory(SimpleMultiDisplay)
add_subdirectory(SimplePlot)
add_subdirectory(SimpleScene)

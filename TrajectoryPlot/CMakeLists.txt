# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin 0.4 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

add_executable(TrajectoryPlot main.cpp)
target_link_libraries(TrajectoryPlot ${Pangolin_LIBRARIES})

add_executable(CameraPlot plot_camera.cpp)
target_link_libraries(CameraPlot ${Pangolin_LIBRARIES})

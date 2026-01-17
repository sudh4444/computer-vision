cmake_minimum_required( VERSION 3.10 )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(HelloSlam)
add_executable(helloSlam helloslam.cpp)
add_library(hello_shared SHARED libHelloSlam.cpp)
target_link_libraries(helloSlam hello_shared)

add_executable(eigen_demo eigenMatrix.cpp)
add_executable(eigen_geo useGeometry.cpp)
add_executable(coordinateTransform coordinateTransform.cpp)



find_package(Pangolin REQUIRED)
add_executable(plotTrajectory plotTrajectory.cpp)
# Use modern CMake target properties (correct variable syntax)
target_include_directories(plotTrajectory PRIVATE ${Pangolin_INCLUDE_DIRS})
target_link_libraries(plotTrajectory PRIVATE ${Pangolin_LIBRARIES})

include_directories("/usr/include/eigen3")
include_directories("/usr/local/include/pangolin")

add_subdirectory("lie_theory")

add_subdirectory("cv_basic")
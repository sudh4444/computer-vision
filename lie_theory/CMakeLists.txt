project(LieTheory)
cmake_minimum_required(VERSION 3.10)

find_package(Sophus REQUIRED)
include_directories(${Sophus_INCLUDE_DIRS})

add_executable(lie_example lie_example.cpp)

find_package(Pangolin REQUIRED)
add_executable(trajectoryError trajectoryError.cpp)
target_include_directories(trajectoryError PRIVATE ${Pangolin_INCLUDE_DIRS})
target_link_libraries(trajectoryError PRIVATE ${Pangolin_LIBRARIES})

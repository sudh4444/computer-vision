project(OpenCVBasics)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Pangolin REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

add_executable(cv_basic cv_basic.cpp)
target_link_libraries(cv_basic ${OpenCV_LIBRARIES})

# install(TARGETS cv_basic DESTINATION bin)

add_executable(undistort undistort.cpp)
target_link_libraries(undistort ${OpenCV_LIBRARIES})

add_executable(stereo stereo.cpp)
target_link_libraries(stereo 
${OpenCV_LIBRARIES}
${Pangolin_LIBRARIES})

add_executable(chessboard_calibrate chessboard_calibrate.cpp)
target_link_libraries(chessboard_calibrate 
${OpenCV_LIBRARIES}
${Pangolin_LIBRARIES})
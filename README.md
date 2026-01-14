# SLAM and Computer Vision Practice

This repository is a comprehensive learning and practice space for **Simultaneous Localization and Mapping (SLAM)**, **computer vision**, **stereo vision**, and **3D reconstruction** techniques.

## Project Overview

The project covers fundamental and advanced topics in visual perception and spatial understanding:

- **Computer Vision Basics**: Image processing, distortion correction, and basic algorithms
- **Stereo Vision**: Stereo matching, depth estimation, and 3D point cloud generation
- **3D Geometry**: Coordinate transformations, rotation matrices, and quaternions
- **SLAM**: Visual localization and mapping techniques
- **Lie Theory**: Advanced mathematical frameworks for rotation representation and optimization

## Directory Structure

- `cv_basic/` - Fundamental computer vision techniques including stereo vision and image undistortion
- `lie_theory/` - Lie group theory implementation using Sophus library
- Build and utility files for compilation and geometric operations

## Key Features

- **Stereo Vision Processing**: Capture and analyze stereo image pairs for depth estimation
- **3D Reconstruction**: Build 3D models from image sequences
- **Image Calibration**: Camera distortion correction and rectification
- **Trajectory Estimation**: Track and estimate pose trajectories
- **Visualization**: Interactive 3D visualization of maps and trajectories

## Building the Project

```bash
./build.sh
```

The build system uses CMake to compile all executable targets including example programs and utilities.

## Learning Topics

- Camera models and calibration
- Epipolar geometry and stereo matching
- Feature detection and matching
- Pose estimation and rigid transformations
- Bundle adjustment and optimization
- 3D visualization and rendering

## Dependencies

- Eigen: Linear algebra
- OpenCV: Computer vision library
- Pangolin: Visualization
- Sophus: Lie group operations

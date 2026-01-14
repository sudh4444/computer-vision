#!/bin/bash

# Check if build folder exists and remove it
if [ -d "build" ]; then
    echo "Removing existing build folder..."
    rm -rf build
fi

# Create new build folder
echo "Creating new build folder..."
mkdir build

# Change to build directory
cd build

# Run cmake
echo "Running cmake..."
cmake ..

# Run make
echo "Running make..."
make

echo "Build completed successfully!"

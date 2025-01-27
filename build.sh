#!/bin/bash

# set a build directory name
BUILD_DIR=build
emcmake cmake -B $BUILD_DIR -S . -DCMAKE_BUILD_TYPE=Release
cmake --build $BUILD_DIR

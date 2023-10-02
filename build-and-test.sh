#!/bin/sh
BUILD_DIR=cmake-build

cmake . -B $BUILD_DIR
cmake --build $BUILD_DIR --target all -j 6
cd $BUILD_DIR
ctest --output-on-failure
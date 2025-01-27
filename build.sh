#!/bin/bash

BUILD_DIR=build

build_em() {
    emcmake cmake -B $BUILD_DIR -S . -DCMAKE_BUILD_TYPE=Release
    cmake --build $BUILD_DIR
}

opencv_build_wasm() {
    cd opencv
    mkdir -p build
    cd build

}


# switch by the first argument
case $1 in
    em)
        build_em
        ;;
    opencv)
        opencv_build_wasm
        ;;
    *)
        echo "Usage: $0 {em|opencv}"
        exit 1
esac

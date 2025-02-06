#!/bin/bash

BUILD_DIR=build

# activate emsdk
source $EMSDK_ENV_PATH/emsdk_env.sh

build_em() {
    rm -rf $BUILD_DIR
    emcmake cmake -B $BUILD_DIR -S . -DCMAKE_BUILD_TYPE=Release
    cmake --build $BUILD_DIR
}

opencv_build_wasm() {
    cd opencv || exit
    mkdir -p build
    emcmake python3 ./platforms/js/build_js.py build --build_wasm
}


# switch by the first argument
case $1 in
    build_em)
        build_em
        ;;
    opencv_build_wasm)
        opencv_build_wasm
        ;;
    *)
        echo "Usage: $0 {build_em|opencv_build_wasm}"
        exit 1
esac

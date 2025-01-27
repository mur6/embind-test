cmake_minimum_required(VERSION 3.10)
project(em-opencv-sample)

# Emscriptenのツールチェインファイルを設定
# set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")

# C++標準を設定
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "opencv/build_js")
find_package(OpenCV REQUIRED)

# 実行可能なJavaScriptを作成
add_executable(opencv_sample_main
    src/main.cpp
    src/image_processing.cpp
)

# OpenCVライブラリをリンク
target_link_libraries(opencv_demo ${OpenCV_LIBS})
target_include_directories(opencv_demo PRIVATE include ${OpenCV_INCLUDE_DIRS})

# Emscriptenフラグを設定
set(EMSCRIPTEN_LINK_FLAGS
    "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXIT_RUNTIME=0 -s EXPORTED_FUNCTIONS='[\"_main\"]' -s EXPORTED_RUNTIME_METHODS='[\"cwrap\"]' --preload-file ${CMAKE_SOURCE_DIR}/assets"
)

set_target_properties(opencv_demo PROPERTIES
    LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
    SUFFIX ".html"
)


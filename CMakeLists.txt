cmake_minimum_required(VERSION 3.10)
project(em-opencv-sample)

set(CMAKE_BUILD_TYPE Debug)

if (NOT EMSCRIPTEN)
    message(FATAL_ERROR "Emscripten toolchain file is not found.")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "opencv/build")
find_package(OpenCV 4 REQUIRED)

# ${PROJECT_NAME}
set(TARGET_NAME  "opencv_sample_main")
# include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(${TARGET_NAME}
    src/main.cpp
)

target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS})
target_include_directories(${TARGET_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

# Emscriptenフラグを設定
#  "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXIT_RUNTIME=0 -s EXPORTED_FUNCTIONS='[\"_main\"]' -s EXPORTED_RUNTIME_METHODS='[\"cwrap\"]' --preload-file ${CMAKE_SOURCE_DIR}/assets"
# string(APPEND EMSCRIPTEN_LINK_FLAGS " -s WASM=1")
string(APPEND EMSCRIPTEN_LINK_FLAGS " -s ALLOW_MEMORY_GROWTH=1")
string(APPEND EMSCRIPTEN_LINK_FLAGS " -s EXIT_RUNTIME=0")
string(APPEND EMSCRIPTEN_LINK_FLAGS " -s MODULARIZE=1")
string(APPEND EMSCRIPTEN_LINK_FLAGS " -s EXPORT_ES6=1")
string(APPEND EMSCRIPTEN_LINK_FLAGS " --bind")

# set_target_properties(opencv_sample_main PROPERTIES
#     LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
#     SUFFIX ".html"
# )


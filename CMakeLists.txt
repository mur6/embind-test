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
    src/resize_by_opencv.cpp
)

target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS})
target_include_directories(${TARGET_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

# Emscriptenフラグを設定
# string(APPEND EMSCRIPTEN_LINK_FLAGS " -s ALLOW_MEMORY_GROWTH=1")
# string(APPEND EMSCRIPTEN_LINK_FLAGS " -s EXIT_RUNTIME=0")
# string(APPEND EMSCRIPTEN_LINK_FLAGS " -s MODULARIZE=1")
# string(APPEND EMSCRIPTEN_LINK_FLAGS " -s EXPORT_ES6=1")
# string(APPEND EMSCRIPTEN_LINK_FLAGS " --bind")
string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ALLOW_MEMORY_GROWTH=1")
string(APPEND CMAKE_EXE_LINKER_FLAGS " -s EXIT_RUNTIME=0")
string(APPEND CMAKE_EXE_LINKER_FLAGS " -s MODULARIZE=1")
string(APPEND CMAKE_EXE_LINKER_FLAGS " -s EXPORT_ES6=1")
string(APPEND CMAKE_EXE_LINKER_FLAGS " --bind")
string(APPEND CMAKE_EXE_LINKER_FLAGS " --preload-file ${CMAKE_SOURCE_DIR}/images@/images")
# string(APPEND CMAKE_EXE_LINKER_FLAGS " --use-preload-plugins")
string(APPEND CMAKE_EXE_LINKER_FLAGS " -fwasm-exceptions")
# set_target_properties(${TARGET_NAME} PROPERTIES
#     LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
#     SUFFIX ".html"
# )

set(CMAKE_EXECUTABLE_SUFFIX ".html")

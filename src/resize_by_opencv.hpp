#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>


emscripten::val cv_cropAndResizeImage(
    const emscripten::val &inputUint8ArrayOfRgba,
    int inputWidth, int inputHeight,
    int outputWidth, int outputHeight);

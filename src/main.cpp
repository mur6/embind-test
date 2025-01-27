#include <opencv2/opencv.hpp>
#include <emscripten/bind.h>

cv::Mat resizeImage(const cv::Mat& input, int width, int height) {
    cv::Mat output;
    cv::resize(input, output, cv::Size(width, height));
    return output;
}

EMSCRIPTEN_BINDINGS(module) {
    emscripten::function("resizeImage", &resizeImage);
    emscripten::class_<cv::Mat>("Mat");
}

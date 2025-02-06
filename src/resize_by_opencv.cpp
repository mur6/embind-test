#include <opencv2/opencv.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>
#include <iostream>


// Function to convert Uint8Array to cv::Mat
cv::Mat convertUint8ArrayToMat(emscripten::val uint8Array, int width, int height) {
    // Get the array length and data pointer
    size_t length = uint8Array["length"].as<size_t>();
    int channels = 4; // RGBA

    // Create a Mat with the specified dimensions and channels
    cv::Mat outputMat(height, width, CV_8UC(channels));

    // Ensure the array size matches expected matrix size
    if (length != static_cast<size_t>(width * height * channels)) {
        throw std::runtime_error("Array size does not match specified dimensions");
    }

    // Copy data from JavaScript Uint8Array to cv::Mat
    emscripten::val heapView = emscripten::val::global("Uint8Array").new_(
        emscripten::typed_memory_view(length, outputMat.data)
    );

    // Set the Mat data from the Uint8Array
    heapView.call<void>("set", uint8Array);

    return outputMat;
}

// 4. C++のcv::MatからJavaScriptのUint8Arrayに変換
emscripten::val convertMatToUint8Array(const cv::Mat &mat) {
    // 1. cv::MatをJavaScriptのUint8Arrayに変換
    size_t length = mat.total() * mat.elemSize();
    emscripten::val uint8Array = emscripten::val::global("Uint8Array").new_(length);
    emscripten::val memoryView = emscripten::val::global("Uint8Array").new_(emscripten::typed_memory_view(length, mat.data));
    uint8Array.call<void>("set", memoryView);
    return uint8Array;
}

cv::Mat _cropAndResizeImage(
    const cv::Mat &inputImage,
    int inputWidth, int inputHeight,
    int outputWidth, int outputHeight) {

    // 2. アスペクト比を維持しながらリサイズ
    int scaledWidth, scaledHeight;
    if (inputWidth * outputHeight < inputHeight * outputWidth) {
        scaledWidth = outputWidth;
        scaledHeight = inputHeight * outputWidth / inputWidth;
    } else {
        scaledHeight = outputHeight;
        scaledWidth = inputWidth * outputHeight / inputHeight;
    }
    std::cout << "Scaled image size: scaledWidth=" << scaledWidth << std::endl;
    std::cout << "Scaled image size: scaledHeight=" << scaledHeight << std::endl;

    cv::Mat resizedImage;
    cv::resize(inputImage, resizedImage, cv::Size(scaledWidth, scaledHeight));
    std::cout << "Resized image size: " << resizedImage.size() << std::endl;

    // 3. 中央部分を切り出し
    int x = (scaledWidth - outputWidth) / 2;
    int y = (scaledHeight - outputHeight) / 2;
    std::cout << "Cropping image at (" << x << ", " << y << ") with size " << outputWidth << "x" << outputHeight << std::endl;
    cv::Rect roi(x, y, outputWidth, outputHeight);
    cv::Mat croppedImage = resizedImage(roi);

    // 4. C++のcv::MatからJavaScriptのUint8Arrayに変換
    return croppedImage;
}

emscripten::val cv_cropAndResizeImage(
    const emscripten::val &inputUint8ArrayOfRgba,
    int inputWidth, int inputHeight,
    int outputWidth, int outputHeight) {
    cv::Mat inputImage = convertUint8ArrayToMat(inputUint8ArrayOfRgba, inputWidth, inputHeight);
    cv::Mat croppedImage = _cropAndResizeImage(inputImage, inputWidth, inputHeight, outputWidth, outputHeight);
    return convertMatToUint8Array(croppedImage);
}

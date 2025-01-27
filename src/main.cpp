#include <opencv2/opencv.hpp>
#include <emscripten/bind.h>



// 画像データを処理するメインメソッド
emscripten::val cropAndResizeImage(
    const emscripten::val &inputUint8ArrayOfRgba,
    int inputWidth, int inputHeight,
    int outputWidth, int outputHeight) {
    // OpenCV用のMatを作成 (RGBAフォーマット)
    // cv::Mat inputImage(inputHeight, inputWidth, CV_8UC4, inputData.data());
    cv::Mat inputImage = convertUint8ArrayToMat(inputUint8ArrayOfRgba, inputWidth, inputHeight);

    // 2. アスペクト比を維持しながらリサイズ
    int scaledWidth, scaledHeight;
    if (inputWidth * outputHeight > inputHeight * outputWidth) {
        scaledWidth = outputWidth;
        scaledHeight = inputHeight * outputWidth / inputWidth;
    } else {
        scaledHeight = outputHeight;
        scaledWidth = inputWidth * outputHeight / inputHeight;
    }

    cv::Mat resizedImage;
    cv::resize(inputImage, resizedImage, cv::Size(scaledWidth, scaledHeight));

    // 3. 中央部分を切り出し
    int x = (scaledWidth - outputWidth) / 2;
    int y = (scaledHeight - outputHeight) / 2;
    cv::Rect roi(x, y, outputWidth, outputHeight);
    cv::Mat croppedImage = resizedImage(roi);

    // 4. C++のcv::MatからJavaScriptのUint8Arrayに変換
    std::vector<uint8_t> outputData(croppedImage.total() * croppedImage.elemSize());
    std::memcpy(outputData.data(), croppedImage.data, outputData.size());

    return emscripten::val::global("Uint8Array").new_(emscripten::val::array(outputData));
}

EMSCRIPTEN_BINDINGS(module) {
    emscripten::function("resizeImage", &resizeImage);
    emscripten::class_<cv::Mat>("Mat");
}

#include <opencv2/opencv.hpp>
#include <emscripten/bind.h>


#ifdef 0
// 画像データを処理するメインメソッド
emscripten::val js_cropAndResizeImage(
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
#endif

// 画像データを処理するメインメソッド
cv::Mat _cropAndResizeImage(
    const cv::Mat &inputImage,
    int inputWidth, int inputHeight,
    int outputWidth, int outputHeight) {

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
    return croppedImage;
}

int main() {
    // 画像の読み込み
    cv::Mat inputImage = cv::imread("input.jpg", cv::IMREAD_COLOR);
    if (inputImage.empty()) {
        std::cerr << "Failed to read image file." << std::endl;
        return 1;
    }

    // 画像の処理
    cv::Mat outputImage = _cropAndResizeImage(inputImage, inputImage.cols, inputImage.rows, 256, 256);

    // 画像の保存
    cv::imwrite("output.jpg", outputImage);

    return 0;
}

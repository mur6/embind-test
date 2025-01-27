#include <opencv2/opencv.hpp>
#include <emscripten/bind.h>

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
    // Create a Uint8Array with the same size as the Mat
    size_t length = mat.total() * mat.elemSize();
    std::vector<uint8_t> outputData(length);
    std::memcpy(outputData.data(), mat.data, length);

    return emscripten::val::global("Uint8Array").new_(emscripten::val::array(outputData));
}

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

extern "C"
{
    // Function to capture the image from the camera
    EMSCRIPTEN_KEEPALIVE
    emscripten::val js_cropAndResizeImage(
        const emscripten::val &inputUint8ArrayOfRgba,
        int inputWidth, int inputHeight,
        int outputWidth, int outputHeight) {
        cv::Mat inputImage = convertUint8ArrayToMat(inputUint8ArrayOfRgba, inputWidth, inputHeight);
        cv::Mat croppedImage = _cropAndResizeImage(inputImage, inputWidth, inputHeight, outputWidth, outputHeight);
        return convertMatToUint8Array(croppedImage);
    }
}


EMSCRIPTEN_BINDINGS(my_module)
{
    emscripten::function("cropAndResizeImage", &js_cropAndResizeImage);
}

int main() {
    EM_ASM({
        console.log("Hello, world!");
        // open /images/xx.jpg
        // cropAndResizeImage
        // save /images/xx_resized.jpg
        FS.mount(MEMFS, {}, '/images');
        FS.readFile('/images/xx.jpg').then(function (data) {
            var inputUint8ArrayOfRgba = new Uint8Array(data);
            var inputWidth = 1920;
            var inputHeight = 1080;
            var outputWidth = 640;
            var outputHeight = 360;
            var outputUint8ArrayOfRgba = Module.cropAndResizeImage(inputUint8ArrayOfRgba, inputWidth, inputHeight, outputWidth, outputHeight);
            FS.writeFile('/images/xx_resized.jpg', outputUint8ArrayOfRgba);
        });
    });
}

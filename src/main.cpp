#include <opencv2/opencv.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>
#include <iostream>

#include "resize_by_opencv.hpp"



// JavaScriptの配列をC++のvectorに変換
std::vector<unsigned char> convertJSArrayToVector(const emscripten::val &jsArray)
{
    const auto length = jsArray["length"].as<unsigned>();
    std::vector<unsigned char> result(length);

    emscripten::val memory = emscripten::val::module_property("HEAPU8");
    emscripten::val memoryView = jsArray["constructor"].new_(memory["buffer"]);

    for (unsigned i = 0; i < length; ++i)
    {
        result[i] = jsArray[i].as<unsigned char>();
    }

    return result;
}

// C++のvectorをJavaScript配列に変換
emscripten::val convertVectorToJSArray(const std::vector<unsigned char> &vec)
{
    emscripten::val Uint8Array = emscripten::val::global("Uint8Array");
    emscripten::val result = Uint8Array.new_(vec.size());

    for (size_t i = 0; i < vec.size(); ++i)
    {
        result.set(i, vec[i]);
    }

    return result;
}

// 切り出しサイズを計算
void calculateCropDimensions(int inputWidth, int inputHeight,
                             double targetAspectRatio,
                             int &cropWidth, int &cropHeight)
{
    if (static_cast<double>(inputWidth) / inputHeight > targetAspectRatio)
    {
        // 入力画像が横長の場合
        cropHeight = inputHeight;
        cropWidth = static_cast<int>(inputHeight * targetAspectRatio);
    }
    else
    {
        // 入力画像が縦長の場合
        cropWidth = inputWidth;
        cropHeight = static_cast<int>(inputWidth / targetAspectRatio);
    }
}

// 画像の切り出しとリサイズを行う
std::vector<unsigned char> cropAndResizeBilinear(
    const std::vector<unsigned char> &input,
    int inputWidth, int inputHeight,
    int startX, int startY,
    int cropWidth, int cropHeight,
    int targetWidth, int targetHeight)
{

    std::vector<unsigned char> result(targetWidth * targetHeight * 4);

    for (int y = 0; y < targetHeight; ++y)
    {
        for (int x = 0; x < targetWidth; ++x)
        {
            // ソース画像での位置を計算
            float srcX = startX + (x * static_cast<float>(cropWidth) / targetWidth);
            float srcY = startY + (y * static_cast<float>(cropHeight) / targetHeight);

            // バイリニア補間で色を計算
            int x1 = static_cast<int>(srcX);
            int y1 = static_cast<int>(srcY);
            int x2 = std::min(x1 + 1, inputWidth - 1);
            int y2 = std::min(y1 + 1, inputHeight - 1);

            float xWeight = srcX - x1;
            float yWeight = srcY - y1;

            for (int c = 0; c < 4; ++c)
            {
                float val =
                    (1 - xWeight) * (1 - yWeight) * input[(y1 * inputWidth + x1) * 4 + c] +
                    xWeight * (1 - yWeight) * input[(y1 * inputWidth + x2) * 4 + c] +
                    (1 - xWeight) * yWeight * input[(y2 * inputWidth + x1) * 4 + c] +
                    xWeight * yWeight * input[(y2 * inputWidth + x2) * 4 + c];

                result[(y * targetWidth + x) * 4 + c] = static_cast<unsigned char>(val);
            }
        }
    }

    return result;
}

// 画像データのりサイズ/クロップ処理: シンプルバージョン
emscripten::val simple_cropAndResizeImage(
    const emscripten::val &inputData,
    int inputWidth, int inputHeight,
    int outputWidth, int outputHeight)
{
    // debug input size, output size
    std::cout << "inputWidth: " << inputWidth << std::endl;
    std::cout << "inputHeight: " << inputHeight << std::endl;
    std::cout << "outputWidth: " << outputWidth << std::endl;
    std::cout << "outputHeight: " << outputHeight << std::endl;

    // 入力データをC++のvectorに変換
    std::vector<unsigned char> imageData = convertJSArrayToVector(inputData);

    // 目標のアスペクト比を計算
    const double targetAspectRatio = static_cast<double>(outputWidth) / outputHeight;

    // 切り出すサイズを計算
    int cropWidth, cropHeight;
    calculateCropDimensions(inputWidth, inputHeight, targetAspectRatio,
                            cropWidth, cropHeight);

    // 切り出し開始位置を計算（中央に配置）
    int startX = (inputWidth - cropWidth) / 2;
    int startY = (inputHeight - cropHeight) / 2;

    // 画像を切り出してリサイズ
    std::vector<unsigned char> croppedData = cropAndResizeBilinear(
        imageData, inputWidth, inputHeight,
        startX, startY, cropWidth, cropHeight,
        outputWidth, outputHeight);
    // std::cout << "croppedData[0]: " << croppedData[0] << std::endl;
    // 結果をJavaScript側に返す
    return convertVectorToJSArray(croppedData);
}


EMSCRIPTEN_BINDINGS(my_module)
{
    emscripten::function("cropAndResizeImage", &simple_cropAndResizeImage);
}

int main() {
    EM_ASM({
        console.log("fs init");
        console.log("fs init done");
        console.log(FS.readdir('/images'));
        FS.readdir('/images').forEach(function (file) {
            console.log(file);
        });
        const uint8Array = FS.readFile('/images/bay.jpg', { encoding: 'binary' });

        // image.src = URL.createObjectURL(new Blob([uint8Array]));
        // JPEG画像をデコードするために、ブラウザのImageオブジェクトを使用する
        const blob = new Blob([uint8Array], { type: 'image/jpeg' });
        createImageBitmap(blob).then((imageBitmap) => {
        const canvas = document.createElement('canvas');
        const inputWidth = 2000;
        const inputHeight = 1123;
        canvas.width = inputWidth;
        canvas.height = inputHeight;
        const ctx = canvas.getContext('2d');
        ctx.drawImage(imageBitmap, 0, 0);
        const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
        console.log('ImageData:', imageData);
        const rgbaData = imageData.data;
        console.log('RGBA Data:', rgbaData);
        const outputWidth = 720;
        const outputHeight = 1280;
        const croppedImage = Module.cropAndResizeImage(rgbaData, inputWidth, inputHeight, outputWidth, outputHeight);
        console.log('Cropped Image:', croppedImage);
        const outputCanvas = document.getElementById('canvas');
        outputCanvas.width = outputWidth;
        outputCanvas.height = outputHeight;
        const outputCtx = outputCanvas.getContext('2d');
        outputCtx.putImageData(new ImageData(new Uint8ClampedArray(croppedImage), outputWidth, outputHeight), 0, 0);
    });
        // document.body.appendChild(image);

    });
}

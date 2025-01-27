#include <opencv2/opencv.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>
#include <iostream>

#include "resize_by_opencv.hpp"

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
    emscripten::function("cropAndResizeImage", &cv_cropAndResizeImage);
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

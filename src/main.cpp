#include <opencv2/opencv.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>
#include <iostream>

#include "resize_by_opencv.hpp"


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

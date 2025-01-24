# Emscripten + OpenCV 画像処理プロジェクト

## プロジェクト構造

```
emscripten-opencv-project/
│
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   └── image_processing.cpp
├── include/
│   └── image_processing.h
└── web/
    └── index.html
```

## ビルド手順

1. 必要な依存関係のインストール:




2. プロジェクトのビルド:

```bash
cd /path/to/emscripten-opencv-project
mkdir build && cd build
emcmake cmake ..
emmake make
```

## 注意点

- OpenCVのパスは環境に合わせて調整してください
- Emscriptenの環境設定が必要です
- ウェブブラウザで実行する必要があります

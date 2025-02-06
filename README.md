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

1. opencvのwasm版のビルド

```sh
sh build.sh opencv_build_wasm
```

1. プロジェクトのビルド

```bash
sh build.sh build_em
```

## 注意点
- Emscriptenの環境設定が必要です
- ウェブブラウザで実行する必要があります

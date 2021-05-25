# wasm-opencv-test
とりあえず、wasmなしでのビルドを試したプロジェクト。

<dl>
<dt>参考サイト:</dt>
<dd>https://bewagner.net/programming/2020/04/12/building-a-face-detector-with-opencv-in-cpp/</dd>
</dl>

# Build
```
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Debug -DOpenCV_DIR={OPENCV_DIR}/build_opencv ../
$ make
```

# Run
```
$ ./OpenCVFaceDetector
OpenCV: not authorized to capture video (status 0), requesting...
OpenCV: camera failed to properly initialize!
```
エラーで実行出来ない！


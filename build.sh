#!/bin/bash

emcmake cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build

#!/bin/bash
# Equivalent to: g++ -o prog main.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL3
mkdir -p build
g++ -o ./build/prog ./src/main.cpp `pkg-config --cflags --libs sdl3`
./build/prog 
#!/bin/bash
# Equivalent to: g++ -o prog main.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL3
g++ -o prog main.cpp `pkg-config --cflags --libs sdl3`
./prog 
## How to install SDL3
[Tutorial](https://www.youtube.com/watch?v=1S5qlQ7U34M)

### Option 1: System Package Manager (Recommended)
```bash
# Ubuntu/Debian
sudo apt install libsdl3-dev
```

### Option 2: Build from Source
```bash
git clone https://github.com/libsdl-org/SDL.git
cd SDL
mkdir build && cd build
cmake ..
make -j 8
sudo make install
```

### Building and running the CHIP-8 Emulator
```bash
./run.sh
```

The project uses pkg-config to automatically find and link against the installed SDL3 library.
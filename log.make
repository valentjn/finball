mkdir -p ./build
g++ src/main.cpp src/renderer/Mesh.cpp src/renderer/Renderer.cpp -O3 -mtune=native -march=native -pedantic -Wall -Wextra -fmessage-length=0 -Wno-unused-parameter -fmessage-length=0 -std=c++14 `pkg-config sdl2 --cflags` `pkg-config bullet --cflags` -I src -I ext -o build/fa_2017_release -lSDL2_image -lGL `pkg-config sdl2 --libs` `pkg-config bullet --libs`
Makefile:32: recipe for target 'release' failed

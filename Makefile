COMMON_CFLAGS=-std=c++11 `pkg-config sdl2 --cflags` `pkg-config bullet --cflags` `pkg-config bullet --libs`
DEBUG_CFLAGS:=-g3 -O0 -pedantic -Wall -Wextra -fmessage-length=0 $(COMMON_CFLAGS)
RELEASE_CFLAGS:= -O3 -mtune=native -march=native -pedantic -Wall -Wextra -fmessage-length=0 $(COMMON_CFLAGS)
LDFLAGS:= -lSDL2_image -lGL `pkg-config sdl2 --libs`

all:	release

install_deb_packages:
	sudo apt-get install libsdl2-image-dev libsdl2-dev

release:
	mkdir -p ./build
	$(CXX) src/main.cpp $(RELEASE_CFLAGS) -o build/fa_2017_release $(LDFLAGS)

debug:
	mkdir -p ./build
	$(CXX) src/main.cpp $(DEBUG_CFLAGS) -o build/fa_2017_debug $(LDFLAGS)

clean:
	rm -rf build Debug Release

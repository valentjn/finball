

DEBUG_CFLAGS:=-g3 -O0 -pedantic -Wall -Wextra -fmessage-length=0 -std=c++11 `pkg-config sdl2 --cflags` -I src
RELEASE_CFLAGS:= -O3 -mtune=native -march=native -pedantic -Wall -Wextra -fmessage-length=0 -std=c++11 `pkg-config sdl2 --cflags` -I src
LDFLAGS:= -lSDL2_image -lGL `pkg-config sdl2 --libs`

all:	release

install_deb_packages:
	sudo apt-get install libsdl2-image-dev libsdl2-dev

release:
	mkdir -p ./build
	$(CXX) $(RELEASE_CFLAGS) src/main.cpp -o build/fa_2017_release $(LDFLAGS)

debug:
	mkdir -p ./build
	$(CXX) $(DEBUG_CFLAGS) src/main.cpp -o build/fa_2017_debug $(LDFLAGS)

clean:
	rm -rf build Debug Release

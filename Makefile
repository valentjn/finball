

DEBUG_CFLAGS:=-g3 -O0 -pedantic -Wall -Wextra -fmessage-length=0 -std=c++11 `pkg-config sdl  --cflags`
RELEASE_CFLAGS:= -O3 -mtune=native -march=native -pedantic -Wall -Wextra -fmessage-length=0 -std=c++11 `pkg-config sdl  --cflags`
LDFLAGS:= -lSDL_image -lv4l2 -lGL `pkg-config sdl  --libs`

all:	release

install_deb_packages:
	sudo apt-get install libsdl-image1.2-dev libsdl1.2-dev libv4l-dev

release:
	mkdir -p ./build
	$(CXX) $(RELEASE_CFLAGS) src/main.cpp src/v4l/imageReader.cpp -o build/fa_2015_release $(LDFLAGS) 

debug:
	mkdir -p ./build
	$(CXX) $(DEBUG_CFLAGS) src/main.cpp src/v4l/imageReader.cpp -o build/fa_2015_debug $(LDFLAGS) 

clean:
	rm -rf build Debug Release

CPP_FILES:= $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
COMMON_CFLAGS:= -pedantic \
		       -Wall \
		       -Wextra \
		       -fmessage-length=0 \
		       -Wno-unused-parameter \
		       -fmessage-length=0 \
		       -std=gnu++14 \
			   -fopenmp \
		       `pkg-config sdl2 --cflags` \
		       `pkg-config bullet --cflags` \
		       -I src \
		       -I ext
KINECT_CFLAGS:= -I/usr/include/ni -I/usr/include/nite
DEBUG_CFLAGS:=-g3 -O0 $(COMMON_CFLAGS)
RELEASE_CFLAGS:= -O3 -mtune=native -march=native $(COMMON_CFLAGS)
OPT_CFLAGS:= -flto -ffast-math -DNDEBUG $(RELEASE_CFLAGS)
LDFLAGS:= -lSDL2_image \
		  -lSDL2_ttf \
		  -lGL \
		  `pkg-config sdl2 --libs` \
		  `pkg-config bullet --libs`
KINECT_LDFLAGS:= -lOpenNI -lXnVNite_1_5_2

.PHONY: test_all

all: release

install_deb_packages:
	sudo apt-get install libsdl2-ttf-dev libsdl2-image-dev libsdl2-dev libbullet-dev

install_formatter:
	sudo apt-get install clang clang-tidy clang-format colordiff

install_icon:
	mkdir -p ~/.local/share/icons
	cp data/haicon.png ~/.local/share/icons
	gtk-update-icon-cache
	mkdir -p ~/.local/share/applications
	cp data/finball.desktop ~/.local/share/applications
	sed -i 's?PWD?'`pwd`'?' ~/.local/share/applications/finball.desktop

release:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(RELEASE_CFLAGS) -o build/fa_2017_release $(LDFLAGS) -D WITHOUT_KINECT_LIBRARIES

optimal:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(OPT_CFLAGS) -o build/fa_2017_release $(LDFLAGS) -D WITHOUT_KINECT_LIBRARIES

debug:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(DEBUG_CFLAGS) -o build/fa_2017_debug $(LDFLAGS) -D WITHOUT_KINECT_LIBRARIES

release-kinect:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(RELEASE_CFLAGS) $(KINECT_CFLAGS) -o build/fa_2017_release $(LDFLAGS) $(KINECT_LDFLAGS)

optimal-kinect:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(OPT_CFLAGS) $(KINECT_CFLAGS) -o build/fa_2017_release $(LDFLAGS) $(KINECT_LDFLAGS)

debug-kinect:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(DEBUG_CFLAGS) $(KINECT_CFLAGS) -o build/fa_2017_debug $(LDFLAGS) $(KINECT_LDFLAGS)

run:
	build/fa_2017_release ${args}

run_verbose:
	build/fa_2017_release -v 10 ${args}

tidy:
	clang-tidy src/main.cpp -- $(COMMON_CFLAGS)
	scan-build -analyze-headers -v make debug

format_diff:
	find src -type f -regex ".*\.\(hpp\|cpp\)" -exec scripts/clang-format-diff {} \;

format:
	find src -type f -regex ".*\.\(hpp\|cpp\)" -exec clang-format -i {} \;

clean:
	rm -rf build

test_all: test_deps
	$(CXX) $(filter-out src/main.cpp,$(CPP_FILES)) test/test_all.cpp $(GTEST_MAIN_CFLAGS) -o build/test $(LDFLAGS)
	build/test

test_test: test_deps
	$(CXX) test/test_test.cpp $(GTEST_MAIN_CFLAGS) -o build/test_test
	build/test_test


GTEST_DIR = ext/googletest/googletest/
GTEST_BUILD_CFLAGS = -I $(GTEST_DIR)/include/ -I $(GTEST_DIR) -pthread
GTEST_BUILD_CFLAGS_MAIN = $(GTEST_DIR)/src/gtest_main.cc $(GTEST_BUILD_CFLAGS)

GTEST_CFLAGS = -I $(GTEST_DIR)/include -pthread build/gtest-all.o
GTEST_MAIN_CFLAGS = $(GTEST_CFLAGS) $(DEBUG_CFLAGS) build/gtest_main.o

build/gtest-all.o:
	mkdir -p build
	$(CXX) $(GTEST_DIR)/src/gtest-all.cc $(GTEST_BUILD_CFLAGS) -c -o $@

build/gtest_main.o:
	mkdir -p build
	$(CXX) $(GTEST_DIR)/src/gtest_main.cc $(GTEST_BUILD_CFLAGS) -c -o $@

test_deps: build/gtest-all.o build/gtest_main.o

# TODO
# GTEST_OBJS = gtest-all gtest_main
# test_dep: $(GTEST_DIR)/src/gtest-all $(GTEST_DIR)/src/gtest_main
# 	mkdir -p build
# 	$(CXX) $(GTEST_DIR)/src/$? $(GTEST_BUILD_CFLAGS) -c -o build/$@.o

CPP_FILES:= $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
COMMON_CFLAGS= -pedantic \
		       -Wall \
		       -Wextra \
		       -fmessage-length=0 \
		       -std=c++11 \
		       `pkg-config sdl2 --cflags` \
		       `pkg-config bullet --cflags` \
		       -I src
DEBUG_CFLAGS:=-g3 -O0 $(COMMON_CFLAGS)
RELEASE_CFLAGS:= -O3 -mtune=native -march=native $(COMMON_CFLAGS)
LDFLAGS:= -lSDL2_image \
		  -lGL \
		  `pkg-config sdl2 --libs` \
		  `pkg-config bullet --libs`

.PHONY: test_all

all: release

install_deb_packages:
	sudo apt-get install libsdl2-image-dev libsdl2-dev libbullet-dev

release:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(RELEASE_CFLAGS) -o build/fa_2017_release $(LDFLAGS)

debug:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(DEBUG_CFLAGS) -o build/fa_2017_debug $(LDFLAGS)

clean:
	rm -rf build

test_all: test_deps
	$(CXX) test/test_all.cpp $(GTEST_MAIN_CFLAGS) -o build/test
	build/test

test_test: test_deps
	$(CXX) test/test_test.cpp $(GTEST_MAIN_CFLAGS) -o build/test_test
	build/test_test



GTEST_DIR = googletest/googletest/
GTEST_BUILD_CFLAGS = -I $(GTEST_DIR)/include/ -I googletest/googletest/ -lpthread
GTEST_BUILD_CFLAGS_MAIN = $(GTEST_DIR)/src/gtest_main.cc $(GTEST_BUILD_CFLAGS)

GTEST_CFLAGS = -I $(GTEST_DIR)/include -lpthread build/gtest-all.o
GTEST_MAIN_CFLAGS = $(GTEST_CFLAGS) build/gtest_main.o

build/gtest-all.o:
	$(CXX) $(GTEST_DIR)/src/gtest-all.cc $(GTEST_BUILD_CFLAGS) -c -o $@

build/gtest_main.o:
	$(CXX) $(GTEST_DIR)/src/gtest_main.cc $(GTEST_BUILD_CFLAGS) -c -o $@

test_deps: build/gtest-all.o build/gtest_main.o

# TODO
# GTEST_OBJS = gtest-all gtest_main
# test_dep: $(GTEST_DIR)/src/gtest-all $(GTEST_DIR)/src/gtest_main
# 	mkdir -p build
# 	$(CXX) $(GTEST_DIR)/src/$? $(GTEST_BUILD_CFLAGS) -c -o build/$@.o

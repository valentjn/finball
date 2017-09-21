CPP_FILES:= $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
COMMON_CFLAGS= -pedantic \
		       -Wall \
		       -Wextra \
		       -fmessage-length=0 \
<<<<<<< HEAD
		       -Wno-unused-parameter \
		       -fmessage-length=0 \
		       -std=c++14 \
		       `pkg-config sdl2 --cflags` \
		       `pkg-config bullet --cflags` \
		       -I src \
		       -I ext
DEBUG_CFLAGS:=-g3 -O0 $(COMMON_CFLAGS)
RELEASE_CFLAGS:= -O3 -mtune=native -march=native $(COMMON_CFLAGS)
OPT_CFLAGS:= -flto -ffast-math -DNDEBUG $(RELEASE_CFLAGS)
=======
		       -std=c++14 \
		       `pkg-config sdl2 --cflags` \
		       `pkg-config bullet --cflags` \
		       -I src
DEBUG_CFLAGS:=-g3 -O0 $(COMMON_CFLAGS)
RELEASE_CFLAGS:= -O3 -mtune=native -march=native $(COMMON_CFLAGS)
>>>>>>> cleanCodeBase-experimental
LDFLAGS:= -lSDL2_image \
		  -lGL \
		  `pkg-config sdl2 --libs` \
		  `pkg-config bullet --libs`

<<<<<<< HEAD
.PHONY: test_all
=======
.PHONY: test
>>>>>>> cleanCodeBase-experimental

all: release

install_deb_packages:
<<<<<<< HEAD
	sudo apt-get install libsdl2-image-dev libsdl2-dev libbullet-dev clang clang-tidy clang-format colordiff
=======
	sudo apt-get install libsdl2-image-dev libsdl2-dev libbullet-dev
>>>>>>> cleanCodeBase-experimental

release:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(RELEASE_CFLAGS) -o build/fa_2017_release $(LDFLAGS)
<<<<<<< HEAD

optimal:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(OPT_CFLAGS) -o build/fa_2017_release $(LDFLAGS)
=======
>>>>>>> cleanCodeBase-experimental

debug:
	mkdir -p ./build
	$(CXX) $(CPP_FILES) $(DEBUG_CFLAGS) -o build/fa_2017_debug $(LDFLAGS)
<<<<<<< HEAD

tidy:
	clang-tidy src/main.cpp -- $(COMMON_CFLAGS)
	scan-build -analyze-headers -v make debug

format-diff:
	find src -type f -regex ".*\.\(hpp\|cpp\)" -not -path "src/glm/*" \
		-exec scripts/clang-format-diff {} \;

format:
	find src -type f -regex ".*\.\(hpp\|cpp\)" -not -path "src/glm/*" \
		-exec clang-format -i {} \;
=======
>>>>>>> cleanCodeBase-experimental

clean:
	rm -rf build

test_all: test_deps
	$(CXX) test/test_all.cpp $(GTEST_MAIN_CFLAGS) -o build/test
	build/test

test_test: test_deps
	$(CXX) test/test_test.cpp $(GTEST_MAIN_CFLAGS) -o build/test_test
	build/test_test



<<<<<<< HEAD
GTEST_DIR = ext/googletest/googletest/
GTEST_BUILD_CFLAGS = -I $(GTEST_DIR)/include/ -I $(GTEST_DIR) -pthread
GTEST_BUILD_CFLAGS_MAIN = $(GTEST_DIR)/src/gtest_main.cc $(GTEST_BUILD_CFLAGS)

GTEST_CFLAGS = -I $(GTEST_DIR)/include -pthread build/gtest-all.o
GTEST_MAIN_CFLAGS = $(GTEST_CFLAGS) build/gtest_main.o

build/gtest-all.o:
	mkdir -p build
	$(CXX) $(GTEST_DIR)/src/gtest-all.cc $(GTEST_BUILD_CFLAGS) -c -o $@

build/gtest_main.o:
	mkdir -p build
=======
GTEST_DIR = googletest/googletest/
GTEST_BUILD_CFLAGS = -I $(GTEST_DIR)/include/ -I googletest/googletest/ -lpthread
GTEST_BUILD_CFLAGS_MAIN = $(GTEST_DIR)/src/gtest_main.cc $(GTEST_BUILD_CFLAGS)

GTEST_CFLAGS = -I $(GTEST_DIR)/include -lpthread build/gtest-all.o
GTEST_MAIN_CFLAGS = $(GTEST_CFLAGS) build/gtest_main.o

build/gtest-all.o:
	$(CXX) $(GTEST_DIR)/src/gtest-all.cc $(GTEST_BUILD_CFLAGS) -c -o $@

build/gtest_main.o:
>>>>>>> cleanCodeBase-experimental
	$(CXX) $(GTEST_DIR)/src/gtest_main.cc $(GTEST_BUILD_CFLAGS) -c -o $@

test_deps: build/gtest-all.o build/gtest_main.o

# TODO
# GTEST_OBJS = gtest-all gtest_main
# test_dep: $(GTEST_DIR)/src/gtest-all $(GTEST_DIR)/src/gtest_main
# 	mkdir -p build
# 	$(CXX) $(GTEST_DIR)/src/$? $(GTEST_BUILD_CFLAGS) -c -o build/$@.o

CPP_FILES:= $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
HPP_FILES:= $(wildcard src/*.hpp) $(wildcard src/**/*.hpp)
RELEASE_OBJ_FILES:= $(patsubst src/%.cpp, build/release/%.o, $(CPP_FILES))
OPT_OBJ_FILES:=     $(patsubst src/%.cpp, build/optimal/%.o, $(CPP_FILES))
DEBUG_OBJ_FILES:=   $(patsubst src/%.cpp, build/debug/%.o,   $(CPP_FILES))

ifdef kinect
KINECT_CFLAGS:= -I/usr/include/ni -I/usr/include/nite
KINECT_LDFLAGS:= -lOpenNI -lXnVNite_1_5_2 -D KINECT_LIBS
endif

ifdef opencv
OPENCV_CFLAGS:= -I/usr/local/include/opencv -I/usr/local/include
OPENCV_LDFLAGS:= -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -D OPENCV_LIBS
endif

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
				 -I ext \
				 -pthread \
				 $(KINECT_CFLAGS) $(OPENCV_CFLAGS)

DEBUG_CFLAGS:=-g3 -O0 $(COMMON_CFLAGS)
RELEASE_CFLAGS:= -O3 -mtune=native -DNDEBUG -march=native $(COMMON_CFLAGS)
OPT_CFLAGS:= -flto -ffast-math -DNDEBUG $(RELEASE_CFLAGS)

LDFLAGS:= -lSDL2_image \
		-lSDL2_ttf \
		-lSDL2_mixer \
		-lGL \
		`pkg-config sdl2 --libs` \
		`pkg-config bullet --libs` \
		$(KINECT_LDFLAGS) $(OPENCV_LDFLAGS)

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

release: $(RELEASE_OBJ_FILES)
	$(CXX) $(RELEASE_CFLAGS) $(RELEASE_OBJ_FILES) $(LDFLAGS) -o build/fa_2017_release

optimal: $(OPT_OBJ_FILES)
	$(CXX) $(OPT_CFLAGS) $(OPT_OBJ_FILES) $(LDFLAGS) -o build/fa_2017_release

debug: $(DEBUG_OBJ_FILES)
	$(CXX) $(DEBUG_CFLAGS) $(DEBUG_OBJ_FILES) $(LDFLAGS) -o build/fa_2017_debug

build/release/%.o: src/%.cpp $(HPP_FILES)
	mkdir -p $(dir $@)
	$(CXX) $(RELEASE_CFLAGS) -c $< -o $@

build/optimal/%.o: src/%.cpp $(HPP_FILES)
	mkdir -p $(dir $@)
	$(CXX) $(OPT_CFLAGS) -c $< -o $@

build/debug/%.o: src/%.cpp $(HPP_FILES)
	mkdir -p $(dir $@)
	$(CXX) $(DEBUG_CFLAGS) -c $< -o $@

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

make_level:
	python3 ./LevelCreator/LevelCreator.py


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

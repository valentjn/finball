#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#include <SDL2/SDL.h>
#include <chrono>

#include "UserInputOutput.hpp"

// standard library includes
#include <memory>

#ifndef WITHOUT_KINECT_LIBRARIES
// forward declarations for kinect
namespace xn {
class Context;
class UserGenerator;
class DepthGenerator;
}

using user_id_t = unsigned long;
#endif

class UserInput {
private:
#ifndef WITHOUT_KINECT_LIBRARIES
    std::unique_ptr<xn::Context> context;
    std::unique_ptr<xn::UserGenerator> userGenerator;
    std::unique_ptr<xn::DepthGenerator> depthGenerator;

	static const int MAX_USERS = 2;
	user_id_t aUsers[MAX_USERS];
	int nUsers;

	std::chrono::high_resolution_clock::time_point previous_time_point;
	float previousLeftAngles[MAX_USERS];
	float previousRightAngles[MAX_USERS];
#endif

public:
    UserInput();
    ~UserInput();

    void getInput(UserInputOutput &userInputOutput);
};

#endif

#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#include <chrono>

#include <SDL2/SDL.h>

#include "UserInput/UserInputOutput.hpp"

// standard library includes
#include <memory>

#ifndef WITHOUT_KINECT_LIBRARIES
// forward declarations for kinect
namespace xn {
class Context;
class UserGenerator;
class DepthGenerator;
}

// typedefs
typedef uint32_t XnUInt32;
typedef XnUInt32 XnUserID;
typedef XnUserID user_id_t;
#endif

class UserInput {
private:
#ifndef WITHOUT_KINECT_LIBRARIES
    std::unique_ptr<xn::Context> context;
    std::unique_ptr<xn::UserGenerator> userGenerator;
    std::unique_ptr<xn::DepthGenerator> depthGenerator;
#endif

	static const int MAX_USERS = 4;
	static const int PLAYERS = 2;
	static constexpr const double MIN_DIFFERENCE = 0.1;
	
	user_id_t players[PLAYERS];
	user_id_t users[MAX_USERS];
	int playerIndices[PLAYERS];
	bool userIsPlayer[MAX_USERS];
	bool playerJoined[PLAYERS];
	int nPlayers, nUsers, trackedUsers;
	float threshold = 0;

	std::chrono::high_resolution_clock::time_point previous_time_point;
	float previousLeftAngles[PLAYERS];
	float previousRightAngles[PLAYERS];

	float max_angle[2] = { 1.5, 1.5 };
	float min_angle[2] = { -1.5, -1.5 };
	float zero_angle[2] = { 0, 0 };

public:
	UserInput();
	~UserInput();
    void getInput(UserInputOutput &userInputOutput);
};

#endif

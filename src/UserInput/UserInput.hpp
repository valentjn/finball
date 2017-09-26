#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#define PI 3.14159265

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
	static const int MAX_USERS = 4;
	static const int PLAYERS = 2;
	static constexpr const double MIN_DIFFERENCE = 0.05;
    static constexpr const double MAX_DIFFERENCE = 0.2;
    static constexpr const double BIG_DIFFERENCE_EASING = 0.1;
    bool leftDifferenceTooBig[PLAYERS];
    bool rightDifferenceTooBig[PLAYERS];

    enum InputSource {
        KEYBOARD
#ifndef WITHOUT_KINECT_LIBRARIES
		, KINECT
#endif
	};
	InputSource usedInputSource;

#ifndef WITHOUT_KINECT_LIBRARIES
    std::unique_ptr<xn::Context> context;
    std::unique_ptr<xn::UserGenerator> userGenerator;
    std::unique_ptr<xn::DepthGenerator> depthGenerator;

	bool kinectIsInitialized = false;
	bool tryInitializingKinect = true;

	user_id_t players[PLAYERS];
	user_id_t users[MAX_USERS];
	int playerIndices[PLAYERS];
	bool userIsPlayer[MAX_USERS];
	bool playerJoined[PLAYERS];
	int nPlayers, nUsers, trackedUsers;
	float threshold = 0;

	bool initializeKinect();
	void finalizeKinect();
#endif

	std::chrono::high_resolution_clock::time_point previous_time_point;
	float previousLeftAngles[PLAYERS];
	float previousRightAngles[PLAYERS];

	float max_angle[PLAYERS];
	float min_angle[PLAYERS];
	float zero_angle[PLAYERS];


	double leftFinStartAngle = 0; // start angle of fins without input
	double rightFinStartAngle = 3.141;
	std::chrono::high_resolution_clock::time_point timeBef;
	std::chrono::high_resolution_clock::time_point timeNow; // time of current angle and velocity
	double a0 = 3.141; // set positive angular acceleration constant
	
	void getSDLInput(UserInputOutput& userInputOutput);

public:
	UserInput();
	~UserInput();
    void getInput(UserInputOutput &userInputOutput);

};

#endif

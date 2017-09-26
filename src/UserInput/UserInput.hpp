#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#define PI 3.14159265

#include <chrono>
#include <string>

#include <SDL2/SDL.h>

#include "UserInput/UserInputOutput.hpp"

// standard library includes
#include <memory>

#ifdef KINECT_LIBS
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
public:
	enum InputSource {
		CHOOSING,
		FAKE,
        KEYBOARD
#ifdef KINECT_LIBS
		, KINECT
#endif
	};

private:
	static const int MAX_USERS = 4;
	static const int PLAYERS = 1;
	static constexpr const double MIN_DIFFERENCE = 0.05;
    static constexpr const double MAX_DIFFERENCE = 0.2;
    static constexpr const double BIG_DIFFERENCE_EASING = 0.1;
	static constexpr const double FAKE_SPEED = 0.001;
    bool leftDifferenceTooBig[PLAYERS];
    bool rightDifferenceTooBig[PLAYERS];

	InputSource usedInputSource;

#ifdef KINECT_LIBS
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
	void getKinectInput(UserInputOutput &userInputOutput, double delta);
#endif

	std::chrono::high_resolution_clock::time_point previous_time_point;
	float previousLeftAngles[PLAYERS];
	float previousRightAngles[PLAYERS];

	float max_angle[PLAYERS];
	float min_angle[PLAYERS];
	float zero_angle[PLAYERS];


	double leftFinStartAngle = 0; // start angle of fins without input
	double rightFinStartAngle = 3.141;
	bool leftPressed = false;
	bool rightPressed = false;

	double aal =0;
	double aar =0;// angular acceleration
	double avl =0; // angular velocity
	double avr =0;
	double anl =leftFinStartAngle; // angle
	double anr =rightFinStartAngle;

	static constexpr const double a0 = 3.141; // set positive angular acceleration constant

	void getSDLInput(UserInputOutput& userInputOutput, double delta);
	void getFakeInput(UserInputOutput& userInputOutput, double delta);

	InputSource getInputSourceFromName(std::string name) {
		if(name == "CHOOSING" || name == "AUTO") {
			return CHOOSING;
		} else if(name == "FAKE") {
			return FAKE;
		} else if(name == "KEYBOARD") {
			return KEYBOARD;
		}
#ifdef KINECT_LIBS
		else if(name == "KINECT") {
			return KINECT;
		}
#endif
	}

public:
	UserInput(InputSource mUsedInputSource = CHOOSING);
	UserInput(std::string mUsedInputSource)
		: UserInput(getInputSourceFromName(mUsedInputSource)) {}
	~UserInput();
    void getInput(UserInputOutput &userInputOutput);

	InputSource getUsedInputSource() { return usedInputSource; }

};

#endif

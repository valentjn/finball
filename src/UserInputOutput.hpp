#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_

#include "SDL2/SDL.h"

class UserInput;

class UserInputOutput {
    double leftAngle[2];
	double rightAngle[2];
public:
    bool quit;

    UserInputOutput() : quit(false) {}

    double getLeftAngle(int player)
    {
		return leftAngle[player-1];
    }
    double getRightAngle(int player)
    {
		return rightAngle[player-1];
    }

	
    friend class UserInput;
};

#endif

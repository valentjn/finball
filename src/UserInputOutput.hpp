#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_

#include "SDL2/SDL.h"

class UserInput;

class UserInputOutput {
    double leftAngle[2];
    double rightAngle[2];
    double leftVelocity[2];
    double rightVelocity[2];
    
public:
    bool quit, start;

    UserInputOutput() : quit(false), start(false) {}

    double getLeftAngle(int player) {
        return leftAngle[player-1];
    }
    double getRightAngle(int player) {
        return rightAngle[player-1];
    }
    double getLeftVelocity(int player) {
        return leftVelocity[player-1];
    }
    double getRightVelocity(int player) {
        return rightVelocity[player-1];
    }

    friend class UserInput;
};

#endif

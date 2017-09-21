#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_

#include "SDL2/SDL.h"

class UserInput;

class UserInputOutput {
    double anglePlayer1, anglePlayer2;
public:
    bool quit;

    UserInputOutput() : quit(false) {}

    double getAngle(int player)
    {
        switch(player){
        case 1: return anglePlayer1;
        case 2: return anglePlayer2;
        }
        return 0;
    }

    friend class UserInput;
};

#endif

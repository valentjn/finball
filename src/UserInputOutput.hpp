#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_

#include "SDL2/SDL.h"

class UserInputOutput {
public:
    bool quit;

    UserInputOutput() :
        quit(false)
    {}
};

#endif

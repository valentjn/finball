#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#include <SDL2/SDL.h>
#include <chrono>

#include "Parameters.hpp"
#include "UserInputOutput.hpp"

class UserInput {
private:
    Parameters &parameters;

public:
    UserInput(Parameters &parameters) : parameters(parameters) {}

    void getInput(UserInputOutput &userInputOutput) {
        // TODO
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                userInputOutput.quit = true;
                break;
            }
        }
		auto time_p = std::chrono::high_resolution_clock::now();
		long t = std::chrono::duration_cast<std::chrono::milliseconds>(time_p.time_since_epoch()).count();
		long steps = 1000;
		userInputOutput.leftAngle[0] = (t%steps)*3.141/steps-6.282;
		userInputOutput.rightAngle[0] = -(t%steps)*3.141/steps+6.282;
		userInputOutput.leftAngle[1] = (t%steps)*3.141/steps-6.282;
		userInputOutput.rightAngle[1] = -(t%steps)*3.141/steps+6.282;
    }
};

#endif

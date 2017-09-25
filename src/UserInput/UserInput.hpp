#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#define PI 3.14159265

#include <chrono>

#include <SDL2/SDL.h>

#include "UserInput/UserInputOutput.hpp"

class UserInput {
private:
    const double maxAngle = PI / 2;
    const double stepSize = maxAngle / 20; // increase of angle with every keydown event

public:
    void getInput(UserInputOutput &userInputOutput) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                userInputOutput.quit = true;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    // 1 keypress is pi/40, velocity starts at pi/10
                    userInputOutput.pressedL -= stepSize;
                    if (userInputOutput.pressedL >= -maxAngle) {
                        userInputOutput.leftAngle[0] = userInputOutput.pressedL;
                        userInputOutput.leftVelocity[0] = 4 * userInputOutput.pressedL;
                    }
                    break;
                case SDLK_RIGHT:
                    // 1 keypress is pi/40, velocity starts at pi/10
                    userInputOutput.pressedR += stepSize;
                    if (userInputOutput.pressedR <= maxAngle) {
                        userInputOutput.rightAngle[0] = userInputOutput.pressedR - PI;
                        userInputOutput.rightVelocity[0] = 4 * userInputOutput.pressedR;
                    }
                    break;
                }
                break;

            case SDL_KEYUP:
                // reset pressedL/R for the next time
                switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    userInputOutput.pressedL = 0;
                    userInputOutput.leftAngle[0] = 0;
                    userInputOutput.leftVelocity[0] = 0;
                    break;
                case SDLK_RIGHT:
                    userInputOutput.pressedR = 0;
                    userInputOutput.rightAngle[0] = -PI;
                    userInputOutput.rightVelocity[0] = 0;
                    break;
                }
                break;
            }
        }

        // auto time_p = std::chrono::high_resolution_clock::now();
        // long t =
        // std::chrono::duration_cast<std::chrono::milliseconds>(time_p.time_since_epoch()).count();
        // long steps = 1000;
        //
        // // generate fake data
        // userInputOutput.leftAngle[0] = (t%steps)*6.282/steps-3.141;
        // userInputOutput.rightAngle[0] = -(t%steps)*6.282/steps+3.141;
        // userInputOutput.leftAngle[1] = (t%steps)*6.282/steps-3.141;
        // userInputOutput.rightAngle[1] = -(t%steps)*6.282/steps+3.141;
        // userInputOutput.leftVelocity[0] = 6.282/(steps/1000.0);
        // userInputOutput.rightVelocity[0] = -6.282/(steps/1000.0);
        // userInputOutput.leftVelocity[1] = 6.282/(steps/1000.0);
        // userInputOutput.rightVelocity[1] = -6.282/(steps/1000.0);

        userInputOutput.playerIsTracked[0] = true;
        userInputOutput.playerIsTracked[1] = true;
        userInputOutput.playerIsCalibrated[0] = true;
        userInputOutput.playerIsCalibrated[1] = true;
        userInputOutput.playerIsCalibrating[0] = false;
        userInputOutput.playerIsCalibrating[1] = false;
        Log::debug("Left: %f, Right: %f", userInputOutput.leftAngle[0],
                   userInputOutput.rightAngle[0]);
        userInputOutput.start = true;
    }
};

#endif

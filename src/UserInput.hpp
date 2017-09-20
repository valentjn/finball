#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#include <SDL2/SDL.h>

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
    }
};

#endif

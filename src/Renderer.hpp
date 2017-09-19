#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <SDL2/SDL.h>

#include "Parameters.hpp"
#include "RendererInput.hpp"

class Renderer {
private:
    Parameters &parameters;
    SDL_Window *window;

public:
    Renderer(Parameters &parameters) : parameters(parameters)
    {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Smiley in SDL",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  500, 500, SDL_WINDOW_SHOWN);
    }

    ~Renderer()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void render(const RendererInput &rendererInput)
    {
        // TODO
    }
};

#endif

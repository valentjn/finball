#ifndef SDL_CONTROLLER_HPP_
#define SDL_CONTROLLER_HPP_

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Log.hpp"

class SDLController {
private:
    SDL_Window *window;
    glm::ivec2 resolution;

    SDLController() : resolution(800, 600) {}

public:
    static SDLController &getInstance() {
        static SDLController instance;
        return instance;
    }

    ~SDLController() {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }

    void startSDL() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            auto error = SDL_GetError();
            Log::error("Failed to initialize SDL: %s", error);
            throw std::runtime_error(error);
        }

        TTF_Init();
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        window = SDL_CreateWindow("Finball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  resolution.x, resolution.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
                                  /* | SDL_WINDOW_FULLSCREEN */);

        if (!window) {
            auto error = SDL_GetError();
            Log::error("Failed to create SDL window: %s", error);
            throw std::runtime_error(error);
        }

        // Set icon
        SDL_Surface *icon_surface = IMG_Load("data/haicon.png");
        SDL_SetWindowIcon(window, icon_surface);
        SDL_FreeSurface(icon_surface);
    }

    SDL_Window *getWindow() {
        return window;
    }

    glm::ivec2 &getResolution() {
        return resolution;
    }

    SDLController(SDLController const &) = delete;
    void operator =(SDLController const &) = delete;
};

#endif

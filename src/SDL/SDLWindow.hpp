#ifndef SDL_WINDOW_HPP_
#define SDL_WINDOW_HPP_

#include <cstdint>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Log.hpp"

class SDLWindow {

private:
    SDL_Window *window;
    int width, height;

public:
    SDLWindow(int width, int height, const char* title, bool fullscreen) {
        handleError(SDL_Init(SDL_INIT_VIDEO) != 0, "Failed to initialize SDL", SDL_GetError());
        handleError(TTF_Init() != 0, "Failed to initialize TTF", TTF_GetError());

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        atexit(SDL_Quit);
        createWindow(width, height, title, fullscreen);

        SDL_GetWindowSize(window, &this->width, &this->height);
    }

    ~SDLWindow() {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    void setIcon(const char* path) {
        SDL_Surface *icon_surface = IMG_Load(path);
        SDL_SetWindowIcon(window, icon_surface);
        SDL_FreeSurface(icon_surface);
    }

    SDL_Window *getWindow() const {
        return window;
    }

private:
    void handleError(bool condition, const char* message, const char* error) {
        if (condition) {
            Log::error("%s: %s", message, error);
            throw std::runtime_error(error);
        }
    }

    void createWindow(int width, int height, const char* title, bool fullscreen) {
        uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        if (fullscreen) {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
            width = height = 0;
        }

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, flags);

        handleError(window == nullptr, "Failed to create SDL Window", SDL_GetError());
    }
};

#endif

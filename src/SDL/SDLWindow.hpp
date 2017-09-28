#ifndef SDL_WINDOW_HPP_
#define SDL_WINDOW_HPP_

#include <cstdint>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Log.hpp"

class SDLWindow {

private:
    SDL_Window *window;
	glm::ivec2 resolution;

public:
    SDLWindow(glm::ivec2 resolution, const char* title, bool fullscreen)
		: resolution(resolution)
	{
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
			Log::error("Failed to initialize SDL: %s", SDL_GetError());

		if (TTF_Init() != 0)
			Log::error("Failed to initialize TTF: %s", TTF_GetError());

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        atexit(SDL_Quit);

        if (fullscreen) {
            SDL_DisplayMode displayMode;
            SDL_GetCurrentDisplayMode(0, &displayMode);
            resolution.x = displayMode.w;
            resolution.y = displayMode.h;
        }
        createWindow(resolution, title, fullscreen);

        Log::info("Window size is %d x %d", resolution.x, resolution.y);
    }

    ~SDLWindow() {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }

	glm::ivec2 getResolution() const
	{
		return resolution;
	}

	int getWidth() const
	{
		return resolution.x;
	}

	int getHeight() const
	{
		return resolution.y;
	}

    void setIcon(const char* path)
	{
        SDL_Surface *icon_surface = IMG_Load(path);
        SDL_SetWindowIcon(window, icon_surface);
        SDL_FreeSurface(icon_surface);
    }

    SDL_Window* getWindow() const
	{
        return window;
    }

private:
    void createWindow(glm::ivec2 resolution, const char* title, bool fullscreen)
	{
        uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        if (fullscreen)
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

        window = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			resolution.x, resolution.y,
			flags);

		if (!window)
			Log::error("Failed to create SDL Window: %s", SDL_GetError());
    }
};

#endif

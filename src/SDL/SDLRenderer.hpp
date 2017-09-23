#ifndef SDL_RENDERER_HPP_
#define SDL_RENDERER_HPP_

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "SDL/SDLWindow.hpp"

class SDLRenderer {

private:
    SDL_Renderer *renderer;
    std::vector<TTF_Font *> fonts;
    int windowWidth;

public:
    SDLRenderer(const SDLWindow &window) {
        renderer = SDL_CreateRenderer(window.getWindow(), -1, SDL_RENDERER_ACCELERATED);
        windowWidth = window.getWidth();
    }

    ~SDLRenderer() {
        for (auto it = fonts.begin(); it != fonts.end(); it++) {
            TTF_CloseFont(*it);
        }

        SDL_DestroyRenderer(renderer);
    }

    TTF_Font *loadFont(const char* path, int ptsize) {
        TTF_Font *font = TTF_OpenFont(path, ptsize);
        fonts.push_back(font);
        return font;
    }

    void addImage(const char* path) {
        SDL_Surface *surface = IMG_Load(path);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_DestroyTexture(texture);
    }

    void addText(std::string text, int x, int y, TTF_Font *font, SDL_Color color, bool center) {
        SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, windowWidth);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        int textWidth, textHeight;
        SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect rect = {x - (center ? textWidth / 2 : 0), y, textWidth, textHeight};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    void render() {
        SDL_RenderPresent(renderer);
    }
};

#endif

#ifndef MAIN_MENU_HPP_
#define MAIN_MENU_HPP_

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class MainMenu {
private:
    SDL_Renderer *renderer;

    SDL_Color colorLight;
    SDL_Color colorDark;

    TTF_Font *headerFont;
    TTF_Font *defaultFont;

    // center = true -> center text >horizontally< on x position
    void createText(std::string text, int x, int y, bool center, SDL_Color color, TTF_Font *font);
    void createText(std::string text, int x, int y, bool center, SDL_Color color) {
        createText(text, x, y, center, color, defaultFont);
    }
    void createText(std::string text, int x, int y, bool center) {
        createText(text, x, y, center, colorDark);
    }
    void createText(std::string text, int x, int y) { createText(text, x, y, true); }

    void setup();
    void hide();

public:
    void show();
};

#endif

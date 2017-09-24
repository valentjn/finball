#ifndef MAIN_MENU_HPP_
#define MAIN_MENU_HPP_

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Level.hpp"
#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"

class MainMenu {
private:
    const SDLWindow &window;
    SDLRenderer renderer;

    TTF_Font *headerFont;
    TTF_Font *defaultFont;

    SDL_Color colorLight;
    SDL_Color colorDark;

public:
    MainMenu(const SDLWindow &window) : window(window), renderer(window) {
        int windowHeight = window.getHeight();
        int headerFontSize = windowHeight * 0.12;
        int defaultFontSize = windowHeight * 0.05;

        headerFont = renderer.loadFont("data/OpenSans-Regular.ttf", headerFontSize);
        defaultFont = renderer.loadFont("data/OpenSans-Regular.ttf", defaultFontSize);

        colorLight = {255, 255, 255, 0};
        colorDark = {50, 50, 50, 0};
    }

    std::unique_ptr<Level> show();

private:
    void setup();

    std::string getHighscoreText();
};

#endif

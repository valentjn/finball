#ifndef GAME_SCREEN_HPP_
#define GAME_SCREEN_HPP_

#include <string>

#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"

using namespace std;

class GameScreen {
private:
    const SDLWindow &window;
    SDLRenderer renderer;

    SDLRenderer::Font headerFont;
    SDLRenderer::Font defaultFont;

    SDLRenderer::Color colorLight;
    SDLRenderer::Color colorDark;

public:
    GameScreen(const SDLWindow &window) : window(window), renderer(window) {
        int windowHeight = window.getHeight();
        int headerFontSize = windowHeight * 0.12;
        int defaultFontSize = windowHeight * 0.05;

        headerFont = renderer.loadFont("data/OpenSans-Regular.ttf", headerFontSize);
        defaultFont = renderer.loadFont("data/OpenSans-Regular.ttf", defaultFontSize);

        colorLight = {255, 255, 255, 0};
        colorDark = {50, 50, 50, 0};
    }

    void addBackgroundImage(const string &path) {
        renderer.addImage(path);
    }

    void addTitle(const string &title) {
        int windowWidth = window.getWidth();
        int windowHeight = window.getHeight();

        renderer.addText(title, windowWidth * 0.5, windowHeight * 0.1,
                headerFont, colorLight, true);
    }

    void addActionText(const string &action) {
        int windowWidth = window.getWidth();
        int windowHeight = window.getHeight();

        renderer.addText(action, windowWidth * 0.5, windowHeight * 0.9,
                defaultFont, colorDark, true);
    }

    void addLeftText(const string &text) {
        int windowWidth = window.getWidth();
        int windowHeight = window.getHeight();

        renderer.addText(text, windowWidth * 0.1, windowHeight * 0.25,
                defaultFont, colorLight, false);
    }

    void render() {
        renderer.render();
    }

};

#endif

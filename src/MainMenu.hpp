#ifndef MAIN_MENU_HPP_
#define MAIN_MENU_HPP_

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "GameController.hpp"
#include "Highscores.hpp"
#include "Level.hpp"
#include "LevelLoader.hpp"
#include "Parameters.hpp"

class MainMenu {
private:
    Parameters parameters;

public:
    MainMenu(Parameters &parameters) : parameters(parameters) {}

    void show() {
        std::vector<float> highscores;
        Highscores::loadHighscores(highscores);

        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        SDL_Window *window =
            SDL_CreateWindow("FinBall", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
                             SDL_WINDOW_SHOWN /* | SDL_WINDOW_FULLSCREEN */);

        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        TTF_Font *header_font = TTF_OpenFont("assets/OpenSans-Regular.ttf", 70);
        TTF_Font *highscore_font = TTF_OpenFont("assets/OpenSans-Regular.ttf", 30);
        SDL_Color color_light = {255, 255, 255, 0};
        SDL_Color color_dark = {50, 50, 50, 0};

        SDL_Surface *icon_surface = IMG_LOAD("assets/haicon.ico");
        SDL_SetWindowIcon(window, icon_surface);

        SDL_Surface *background_surface = IMG_Load("assets/background.jpg");
        SDL_Texture *background_texture =
            SDL_CreateTextureFromSurface(renderer, background_surface);

        SDL_Surface *header_surface = TTF_RenderText_Blended_Wrapped(header_font, "FinBall", color_light, 1000);
        SDL_Texture *header_texture = SDL_CreateTextureFromSurface(renderer, header_surface);

        std::string highscore_text = get_highscore_text(highscores);

        SDL_Surface *highscore_surface = TTF_RenderText_Blended_Wrapped(
            highscore_font, highscore_text.c_str(), color_light, 1000);
        SDL_Texture *highscore_texture = SDL_CreateTextureFromSurface(renderer, highscore_surface);

        SDL_Surface *start_game_surface = TTF_RenderText_Blended_Wrapped(
            highscore_font, "To start the game please do a HAI-five!", color_dark, 1000);
        SDL_Texture *start_game_texture =
            SDL_CreateTextureFromSurface(renderer, start_game_surface);

        SDL_FreeSurface(icon_surface);
        SDL_FreeSurface(background_surface);
        SDL_FreeSurface(header_surface);
        SDL_FreeSurface(highscore_surface);
        SDL_FreeSurface(start_game_surface);

        int textWidth, textHeight;
        SDL_QueryTexture(header_texture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect header_rect = {400 - textWidth / 2, 50, textWidth, textHeight};
        SDL_QueryTexture(highscore_texture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect highscore_rect = {600, 150, textWidth, textHeight};
        SDL_QueryTexture(start_game_texture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect start_game_rect = {400 - textWidth / 2, 500, textWidth, textHeight};

        SDL_RenderCopy(renderer, background_texture, NULL, NULL);
        SDL_RenderCopy(renderer, header_texture, NULL, &header_rect);
        SDL_RenderCopy(renderer, highscore_texture, NULL, &highscore_rect);
        SDL_RenderCopy(renderer, start_game_texture, NULL, &start_game_rect);
        SDL_RenderPresent(renderer);

        // Flag for quitting the program
        bool quit = false;

        SDL_Event event;

        while (!quit) {
            SDL_PollEvent(&event);

            switch (event.type) {
            case SDL_QUIT: // User hits the "x" in the corner of the window
                quit = true;
                break;
            }
        }

        // Free resources
        TTF_CloseFont(header_font);
        SDL_DestroyTexture(background_texture);
        SDL_DestroyTexture(header_texture);
        SDL_DestroyTexture(highscore_texture);
        SDL_DestroyTexture(start_game_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        // Quit SDL
        TTF_Quit();
        SDL_Quit();

        Level level;
        LevelLoader levelLoader(parameters);
        levelLoader.loadLevel(level);

        GameController gameController(parameters);
        gameController.startGame(level);
    }

private:
    std::string get_highscore_text(std::vector<float> &highscores) {
        std::stringstream stream;
        int counter = 1;

        stream << std::fixed << std::setprecision(2);

        for (auto it = highscores.begin(); it != highscores.end() && counter <= 5; it++) {
            stream << "(" << counter++ << ") " << *it << "\n";
        }

        return stream.str();
    }
};

#endif

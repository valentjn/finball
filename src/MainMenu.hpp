#ifndef MAIN_MENU_HPP_
#define MAIN_MENU_HPP_

#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Parameters.hpp"
#include "Level.hpp"
#include "LevelLoader.hpp"
#include "GameController.hpp"

class MainMenu {
private:
    Parameters parameters;

public:
    MainMenu(Parameters &parameters) : parameters(parameters) {}

    void show() {
        std::vector<float> highscores;
        load_highscores(highscores);

        
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();
        SDL_Window *window = SDL_CreateWindow("FinBall", SDL_WINDOWPOS_UNDEFINED, 
                                              SDL_WINDOWPOS_UNDEFINED, 800, 600,
                                              SDL_WINDOW_SHOWN /* | SDL_WINDOW_FULLSCREEN */);


        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        TTF_Font *header_font = TTF_OpenFont("assets/OpenSans-Regular.ttf", 70);
        TTF_Font *highscore_font = TTF_OpenFont("assets/OpenSans-Regular.ttf", 30);
        SDL_Color color = { 255, 255, 255, 0 };
        
        SDL_Surface *background_surface = IMG_Load("assets/background.jpg");
        SDL_Texture *background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);

        SDL_Surface *header_surface = TTF_RenderText_Solid(header_font, "FinBall", color);
        SDL_Texture *header_texture = SDL_CreateTextureFromSurface(renderer, header_surface);

        std::string highscore_text = get_highscore_text(highscores);

        SDL_Surface *highscore_surface = TTF_RenderText_Blended_Wrapped(highscore_font, highscore_text.c_str(), color, 1000);
        SDL_Texture *highscore_texture = SDL_CreateTextureFromSurface(renderer, highscore_surface);

        SDL_FreeSurface(background_surface);
        SDL_FreeSurface(header_surface);
        SDL_FreeSurface(highscore_surface);

        int textWidth, textHeight;
        SDL_QueryTexture(header_texture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect header_rect = { 400 - textWidth / 2 , 50, textWidth, textHeight };
        SDL_QueryTexture(highscore_texture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect highscore_rect = { 600, 150, textWidth, textHeight };

        SDL_RenderCopy(renderer, background_texture, NULL, NULL);
        SDL_RenderCopy(renderer, header_texture, NULL, &header_rect);
        SDL_RenderCopy(renderer, highscore_texture, NULL, &highscore_rect);
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
    void load_highscores(std::vector<float> &highscores) {
        highscores.push_back(1.23);
        highscores.push_back(7.65);
        highscores.push_back(1.23456789);
        highscores.push_back(5.1243);
    }

    std::string get_highscore_text(std::vector<float> &highscores) {
         std::stringstream stream;
         int counter = 1;

         stream << std::fixed << std::setprecision(2);

         for (auto it = highscores.begin(); it != highscores.end(); it++) {
              stream << counter++ << ". " << *it << "\n";
         }

         return stream.str();
    }

};

#endif

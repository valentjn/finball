#ifndef MAIN_MENU_HPP_
#define MAIN_MENU_HPP_

#include <vector>

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
        TTF_Font *font = TTF_OpenFont("assets/OpenSans-regular.ttf", 25);
        SDL_Color color = { 255, 255, 255 };
        
        SDL_Surface *background_surface = IMG_Load("assets/background.jpg");
        SDL_Texture *background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);

        SDL_Surface *highscore_surface = TTF_RenderText_Solid(font, "FinBall - The Game", color);
        SDL_Texture *highscore_texture = SDL_CreateTextureFromSurface(renderer, highscore_surface);

        SDL_FreeSurface(background_surface);
        SDL_FreeSurface(highscore_surface);

        SDL_RenderCopy(renderer, background_texture, NULL, NULL);
        SDL_RenderCopy(renderer, highscore_texture, NULL, NULL);
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
        TTF_CloseFont(font);
        SDL_DestroyTexture(background_texture);
        SDL_DestroyTexture(highscore_texture);
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
        // read file
        // write to vector
        // sort
    }

};

#endif

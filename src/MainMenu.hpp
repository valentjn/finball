#ifndef MAIN_MENU_HPP_
#define MAIN_MENU_HPP_

class MainMenu {

public:
    void show() {
        std::vector<float> highscores;
        load_hoghscores(highscores);

        
        // Initialize SDL with the video subsystem
        SDL_Init(SDL_INIT_VIDEO);


        // Create a window on the screen
        SDL_Window *window = SDL_CreateWindow("FinBall", SDL_WINDOWPOS_UNDEFINED, 
                                              SDL_WINDOWPOS_UNDEFINED, 0, 0,
                                              SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);


        // Create the renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


        // Create a surface for loading an image 
        SDL_Surface* surface = IMG_Load("images/background.jpg");


        // Create the texture from the surface
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);


        // Free the surface
        SDL_FreeSurface(surface);
          
            
        // Let the renderer render the texture
        SDL_RenderCopy(renderer, texture, NULL, NULL);
            
            
        // Move the image from the back buffer to the front buffer
        SDL_RenderPresent(renderer);
            
        
        // Flag for quitting the program
        bool quit = false;
            
            
        // SDL event
        SDL_Event event;


        // Game loop: this is the main loop of any SDL program
        while (!quit) {
            
            // Check for SDL events
            SDL_PollEvent(&event);
                
            switch (event.type) {
                case SDL_QUIT: // User hits the "x" in the corner of the window
                    quit = true;
                    break;
            }
        }
            
        // Free resources
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
            
        // Quit SDL
        SDL_Quit();
    }

private:
    void load_highscores(std::vector<float> &highscores) {
        // read file
        // write to vector
        // sort
    }

}

#endif

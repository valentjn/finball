#include <iomanip>
#include <sstream>

#include "GameController.hpp"
#include "Highscores.hpp"
#include "Level.hpp"

#include "MainMenu.hpp"

std::string getHighscoreText() {
    std::vector<float> highscores;
    Highscores::loadHighscores(highscores);

    std::stringstream stream;
    int counter = 1;

    stream << std::fixed << std::setprecision(2);
    for (auto it = highscores.begin(); it != highscores.end() && counter <= 5; it++) {
        stream << "(" << counter++ << ") " << *it << "\n";
    }
    return stream.str();
}

// center = true -> center text >horizontally< on x position
void MainMenu::createText(std::string text, int x, int y, bool center, SDL_Color color,
                          TTF_Font *font) {
    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, 1000);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    int textWidth, textHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect rect = {x - (center ? textWidth / 2 : 0), y, textWidth, textHeight};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

void MainMenu::show() {
    setup();

    // Flag for quitting the program
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN: // if key is "SPACE" it falls through and quits the menu
                if (event.key.keysym.sym != SDLK_SPACE) {
                    break;
                }
            case SDL_QUIT: // User hits the "x" in the corner of the window
                quit = true;
                break;
            }
        }
    }

    hide();
    Level level("data/testLevel.txt");
    GameController gameController;
    gameController.startGame(level);
}

void MainMenu::setup() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow("FinBall", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
                              SDL_WINDOW_SHOWN /* | SDL_WINDOW_FULLSCREEN */);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Setup fonts and colors
    colorDark = {50, 50, 50, 0};
    colorLight = {255, 255, 255, 0};
    headerFont = TTF_OpenFont("data/OpenSans-Regular.ttf", 70);
    defaultFont = TTF_OpenFont("data/OpenSans-Regular.ttf", 30);

    // Set icon
    SDL_Surface *icon_surface = IMG_Load("data/haicon.png");
    SDL_SetWindowIcon(window, icon_surface);
    SDL_FreeSurface(icon_surface);

    // Set background
    SDL_Surface *background_surface = IMG_Load("data/background.jpg");
    SDL_Texture *background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);
    SDL_FreeSurface(background_surface);
    SDL_RenderCopy(renderer, background_texture, NULL, NULL);
    SDL_DestroyTexture(background_texture);

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    createText("FinBall", windowWidth / 2, 50, true, colorLight, headerFont);
    createText("To start the game please do a HAI-five or press SPACE!", windowWidth / 2, 500);

    std::string highscore_text = getHighscoreText();
    createText(highscore_text, 600, 150, false, colorLight);

    SDL_RenderPresent(renderer);
}

void MainMenu::hide() {
    // Free resources
    TTF_CloseFont(headerFont);
    TTF_CloseFont(defaultFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quit SDL
    TTF_Quit();
    SDL_Quit();
}

#include "Log.hpp"
#include "MainMenu.hpp"
#include "Parameters.hpp"
#include "MusicPlayer.hpp"

int main(int argc, char *argv[]) {
    Parameters parameters(argc, argv);
    Log::setLogLevel(parameters.verbosityLevel);

    MusicPlayer musicPlayer;
    musicPlayer.play("data/music.flac");

    MainMenu menu;
    menu.show();

    /*
    Level level("data/testLevel.txt");
    GameController gameController;
    gameController.startGame(level);
    */

    return 0;
}

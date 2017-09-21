<<<<<<< HEAD
#include "GameController.hpp"
#include "Level.hpp"
#include "LevelLoader.hpp"

Parameters parameters;

int main(int argc, char *argv[]) {
    parameters.setup(argc, argv);

    Level level;
    LevelLoader levelLoader(parameters);
    levelLoader.loadLevel(level);

    GameController gameController(parameters);
    gameController.startGame(level);

    return 0;
=======
#include <GameController.hpp>
#include <renderer/CompRenderer.hpp>
#include <ui/CompUI.hpp>
#include <gamelogic/CompGameLogic.hpp>
#include <interaction/InterGameLogicToRenderer.hpp>

int main()
{
    GameController game;

    auto ui = game.addComponent<CompUI>();
    auto game_logic = game.addComponent<CompGameLogic>();
    auto renderer = game.addComponent<CompRenderer>();

    game.addInteraction<InterGameLogicToRenderer>(game_logic, renderer);
    
    game.run();
>>>>>>> cleanCodeBase-experimental
}


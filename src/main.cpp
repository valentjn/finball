#include <GameController.hpp>
#include <renderer/CompRenderer.hpp>
#include <ui/CompUI.hpp>
#include <gamelogic/CompGameLogic.hpp>
#include <interaction/InterGameLogicToRenderer.hpp>
#include <interaction/InterLBMToRenderer.hpp>
#include <lbm/CompLatticeBoltzmann.hpp>
#include <Level.hpp>
#include <Parameters.hpp>
#include <LevelLoader.hpp>
#include <MainMenu.hpp>

int main(int argc, char* argv[])
{

    Parameters parameters;
    parameters.setup(argc, argv);
    Level level;
    LevelLoader levelLoader(parameters);
    levelLoader.loadLevel(level);

    MainMenu menu(parameters);
    menu.show();

    GameController game{ level };

    auto lbm = game.addComponent<CompLatticeBoltzmann>();
    auto ui = game.addComponent<CompUI>();
    auto game_logic = game.addComponent<CompGameLogic>();
    auto renderer = game.addComponent<CompRenderer>();

    game.addInteraction<InterGameLogicToRenderer>(game_logic, renderer);
    game.addInteraction<InterLBMToRenderer>(lbm, renderer);

    game.run();
}


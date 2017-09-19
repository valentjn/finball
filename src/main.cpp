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
}


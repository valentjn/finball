#include <GameController.hpp>
#include <renderer/CompRenderer.hpp>
#include <gamelogic/CompGameLogic.hpp>

int main()
{
    GameController game;

    auto game_logic = game.addComponent<CompGameLogic>();
    auto renderer = game.addComponent<CompRenderer>();

    game.addInteraction<InterGameLogicToRenderer>(game_logic, renderer);
    
    game.run();

	/*while (true) {
		// Poll available events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				return;
			}
		}

		// Render
		renderer.renderWorldObject(object);
		renderer.present();

		// Delay 15ms
		SDL_Delay(15);
	}*/
}


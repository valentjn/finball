#include "GameController.hpp"

Parameters parameters;

int main(int argc, char *argv[])
{
	parameters.setup(argc, argv);

	GameController gameController(parameters);
	gameController.startGame();

	return 0;
}

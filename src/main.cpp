#include "Array2D.hpp"
#include "LevelLoader.hpp"
#include "GameController.hpp"

Parameters parameters;

int main(int argc, char *argv[])
{
	parameters.setup(argc, argv);

	LevelLoader levelLoader(parameters);
	Array2D<LevelLoader::CellType> *level = levelLoader.loadLevel("testLevel");

	GameController gameController(parameters);
	gameController.startGame(level);

	if (level != nullptr)
	{
		delete level;
	}

	return 0;
}

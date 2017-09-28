#ifndef GAME_LOGIC_INPUT_HPP_
#define GAME_LOGIC_INPUT_HPP_

#include <vector>

#include <UserInput/UserInputOutput.hpp>
#include <Physics/Physics.hpp>

class GameLogicInput {
public:
    bool quit;
    const std::vector<Transform*> *rigidBodies;
    const Array2D<Level::CellType> *grid_objects;

    GameLogicInput() {}

    void process(const UserInputOutput &userInputOutput, const PhysicsOutput& physicsOutput)
	{
        quit = userInputOutput.quit;
        rigidBodies = &physicsOutput.rigidBodyPhysicsOutput.rigid_bodies;
        grid_objects = &physicsOutput.rigidBodyPhysicsOutput.grid_objects;
    }
};

#endif

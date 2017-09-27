#ifndef GAME_LOGIC_INPUT_HPP_
#define GAME_LOGIC_INPUT_HPP_

#include <vector>

#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "RigidBody/Transform.hpp"
#include "UserInput/UserInputOutput.hpp"

class GameLogicInput {
public:
    bool quit;
    const std::vector<Transform*> *rigidBodies;

    GameLogicInput() {}

    void process(
		const UserInputOutput &userInputOutput,
        const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
        const LatticeBoltzmannOutput &latticeBoltzmannOutput)
	{
        quit = userInputOutput.quit;
        rigidBodies = &rigidBodyPhysicsOutput.rigid_bodies;
    }
};

#endif

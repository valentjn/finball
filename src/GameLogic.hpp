#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include "Parameters.hpp"
#include "UserInputData.hpp"
#include "RigidBodyData.hpp"
#include "LatticeBoltzmannData.hpp"
#include "GameLogicData.hpp"

class GameLogic {
private:
    Parameters &parameters;

public:
    GameLogic(Parameters &parameters) : parameters(parameters)
    {}

    void update(const UserInputData *userInputData,
                const RigidBodyData *rigidBodyData,
                const LatticeBoltzmannData *latticeBoltzmannData,
                GameLogicData *gameLogicData)
    {
        // TODO
    }
};

#endif

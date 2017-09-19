#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include "Parameters.hpp"
#include "GameLogicInput.hpp"
#include "GameLogicOutput.hpp"

class GameLogic {
private:
    Parameters &parameters;

public:
    GameLogic(Parameters &parameters) : parameters(parameters)
    {}

    void update(const GameLogicInput *input,
                GameLogicOutput *gameLogicOutput)
    {
        // TODO
    }
};

#endif

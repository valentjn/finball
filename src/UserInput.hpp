#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#include "Parameters.hpp"
#include "UserInputOutput.hpp"

class UserInput {
private:
    Parameters &parameters;

public:
    UserInput(Parameters &parameters) : parameters(parameters)
    {}

    void getInput(UserInputOutput &userInputOutput)
    {
        // TODO
    }
};

#endif

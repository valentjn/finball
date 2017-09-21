#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#include "Parameters.hpp"
#include "UserInputOutput.hpp"

// standard library includes
#include <memory>

// forward declarations for kinect
namespace xn
{
class Context;
}

class UserInput {
private:
    Parameters &parameters;
    std::unique_ptr<xn::Context> context;

public:
    UserInput(Parameters &parameters);
    ~UserInput();

    void getInput(UserInputOutput &userInputOutput);
};

#endif

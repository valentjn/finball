#include "UserInput.hpp"

// headers for kinect
#include <XnCppWrapper.h>

UserInput::UserInput(Parameters &parameters)
    : parameters(parameters), context(std::make_unique<xn::Context>())
{
    //TODO
}

UserInput::~UserInput()
{
    //TODO
    context->Release();
}

void UserInput::getInput(UserInputOutput &userInputOutput)
{
    //TODO
}

#include "UserInput.hpp"

// headers for kinect
#include <XnCppWrapper.h>

using namespace std;
using namespace xn;

UserInput::UserInput(Parameters &parameters)
    : parameters(parameters),
      context(make_unique<Context>()),
      userGenerator(make_unique<UserGenerator>()),
      depthGenerator(make_unique<DepthGenerator>())
{
    //TODO: handle error codes
    XnStatus errorCode = XN_STATUS_OK;

    errorCode = context->Init();
    errorCode = depthGenerator->Create(*context);

    // start generating data
    errorCode = context->StartGeneratingAll();
}

UserInput::~UserInput()
{
    //TODO
    depthGenerator->Release();
    userGenerator->Release();
    context->Release();
}

void UserInput::getInput(UserInputOutput &userInputOutput)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            userInputOutput.quit = true;
            break;
        }
    }

    //TODO: handle error codes
    XnStatus errorCode = XN_STATUS_OK;

    context->WaitNoneUpdateAll();
    //TODO
}

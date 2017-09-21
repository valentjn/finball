#include "UserInput.hpp"

#ifndef WITHOUT_KINECT_LIBRARIES

// headers for kinect
#include <XnCppWrapper.h>

using namespace xn;

#endif

#include <stdexcept>

using namespace std;

UserInput *UserInput::theUserInput = nullptr;

UserInput::UserInput(Parameters &parameters)
    : parameters(parameters) {
#ifndef WITHOUT_KINECT_LIBRARIES
    context = make_unique<Context>();
    userGenerator = make_unique<UserGenerator>();
    depthGenerator = make_unique<DepthGenerator>();
#endif

    if(theUserInput!=nullptr) {
        throw std::logic_error(
            "Singleton UserInput instantiated multiple times");
    }
    theUserInput = this;

#ifndef WITHOUT_KINECT_LIBRARIES
    // TODO: handle error codes
    XnStatus errorCode = XN_STATUS_OK;

    errorCode = context->Init();
    errorCode = depthGenerator->Create(*context);

    // start generating data
    errorCode = context->StartGeneratingAll();
#endif
}

UserInput::~UserInput() {
    // TODO
#ifndef WITHOUT_KINECT_LIBRARIES
    depthGenerator->Release();
    userGenerator->Release();
    context->Release();
#endif
}

void UserInput::getInput(UserInputOutput &userInputOutput) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            userInputOutput.quit = true;
            break;
        }
    }

#ifndef WITHOUT_KINECT_LIBRARIES
    // TODO: handle error codes
    XnStatus errorCode = XN_STATUS_OK;

    context->WaitNoneUpdateAll();
    // TODO
#endif
}

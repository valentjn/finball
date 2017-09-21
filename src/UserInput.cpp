#include "UserInput.hpp"

#ifndef WITHOUT_KINECT_LIBRARIES

// headers for kinect
#include <XnCppWrapper.h>

using namespace xn;

#endif

#include <stdexcept>

using namespace std;

// initialization of user input facilities
UserInput::UserInput(Parameters &parameters) : parameters(parameters) {

#ifndef WITHOUT_KINECT_LIBRARIES
    // init kinect context etc
    context = make_unique<Context>();
    userGenerator = make_unique<UserGenerator>();
    depthGenerator = make_unique<DepthGenerator>();

    // TODO: handle error codes
    XnStatus errorCode = XN_STATUS_OK;

    errorCode = context->Init();
    errorCode = context->FindExistingNode(XN_NODE_TYPE_DEPTH, *depthGenerator);
    if(errorCode != XN_STATUS_OK) {
        errorCode = depthGenerator->Create(*context);
    }

    errorCode = context->FindExistingNode(XN_NODE_TYPE_USER, *userGenerator);
    if(errorCode != XN_STATUS_OK) {
        errorCode = userGenerator->Create(*context);
    }

    // register callbacks
    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete;

    errorCode = userGenerator->RegisterUserCallbacks(
        [](UserGenerator& userGenerator, XnUserID id, void* cookie){
            UserInput* _this = reinterpret_cast<UserInput*>(cookie);
            // new user added.
        },
        [](UserGenerator& userGenerator, XnUserID id, void* cookie){
            UserInput* _this = reinterpret_cast<UserInput*>(cookie);
            // user lost
        },
        this, hUserCallbacks);

    errorCode = userGenerator->GetSkeletonCap().RegisterToCalibrationStart(
        [](SkeletonCapability& skeletonCapability, XnUserID nID, void* cookie){
            UserInput* _this = reinterpret_cast<UserInput*>(cookie);
            // calibration start
        }, this, hCalibrationStart);

    errorCode = userGenerator->GetSkeletonCap().RegisterToCalibrationComplete(
        [](SkeletonCapability& skeletonCapability, XnUserID nID,
            XnCalibrationStatus status,
            void* cookie){
            UserInput* _this = reinterpret_cast<UserInput*>(cookie);
            // calibration complete
        }, this, hCalibrationComplete);

    // Get Skeleton
    userGenerator->GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    // start generating data
    errorCode = context->StartGeneratingAll();
#endif
}

// finalization of user input facilities
UserInput::~UserInput() {
    // TODO
#ifndef WITHOUT_KINECT_LIBRARIES
    // cleanup kinect context etc
    depthGenerator->Release();
    userGenerator->Release();
    context->Release();
#endif
}

// process input
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

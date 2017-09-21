#include "UserInput.hpp"

#ifndef WITHOUT_KINECT_LIBRARIES

// headers for kinect
#include <XnCppWrapper.h>

using namespace xn;

#endif

#include <stdexcept>
#include <cmath>

using namespace std;
using namespace std::chrono;

// initialization of user input facilities
UserInput::UserInput(Parameters &parameters) : parameters(parameters) {

#ifndef WITHOUT_KINECT_LIBRARIES
    // init kinect context etc
    context = make_unique<Context>();
    userGenerator = make_unique<UserGenerator>();
    depthGenerator = make_unique<DepthGenerator>();
    previous_time_point = std::chrono::high_resolution_clock::now();

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
        [](UserGenerator& userGenerator, XnUserID nID, void* cookie){
            UserInput* _this = reinterpret_cast<UserInput*>(cookie);
            // new user added.
            userGenerator.GetSkeletonCap().RequestCalibration(nID, TRUE);
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
            if(status == XN_CALIBRATION_STATUS_OK) {
                _this->userGenerator->GetSkeletonCap().StartTracking(nID);
            } else {
                // TODO: handle errors
                _this->userGenerator->GetSkeletonCap()
                    .RequestCalibration(nID, TRUE);
            }
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

    auto now = high_resolution_clock::now();
    milliseconds delta = duration_cast<milliseconds>(now-previous_time_point);

    for(XnUInt16 i=0; i<nUsers; i++) {
        if(userGenerator->GetSkeletonCap().IsTracking(aUsers[i])==FALSE) {
            continue;
        }

        XnSkeletonJointTransformation leftHandJoint, leftElbowJoint;
        XnSkeletonJointTransformation rightHandJoint, rightElbowJoint;

        userGenerator->GetSkeletonCap().GetSkeletonJoint(
            aUsers[i], XN_SKEL_LEFT_HAND, leftHandJoint);
        userGenerator->GetSkeletonCap().GetSkeletonJoint(
            aUsers[i], XN_SKEL_LEFT_ELBOW, leftElbowJoint);
        userGenerator->GetSkeletonCap().GetSkeletonJoint(
            aUsers[i], XN_SKEL_RIGHT_HAND, rightHandJoint);
        userGenerator->GetSkeletonCap().GetSkeletonJoint(
            aUsers[i], XN_SKEL_RIGHT_ELBOW, rightElbowJoint);

		// compute angles
        double left_x_diff_1 = leftHandJoint.position.position.X
			- leftElbowJoint.position.position.X;
		double left_y_diff_1 = leftHandJoint.position.position.Y
			- leftElbowJoint.position.position.Y;
		userInputOutput.rightAngle[0] = atan2(left_y_diff_1, left_x_diff_1);
		
		double right_x_diff_1 = leftHandJoint.position.position.X
			- leftElbowJoint.position.position.X;
		double right_y_diff_1 = leftHandJoint.position.position.Y
			- leftElbowJoint.position.position.Y;
		userInputOutput.rightAngle[0] = atan2(left_y_diff_1, left_x_diff_1);		

		double left_x_diff_2 = leftHandJoint.position.position.X
			- leftElbowJoint.position.position.X;
		double left_y_diff_2 = leftHandJoint.position.position.Y
			- leftElbowJoint.position.position.Y;
		userInputOutput.leftAngle[1] = atan2(left_y_diff_1, left_x_diff_1);
		
		double right_x_diff_2 = leftHandJoint.position.position.X
			- leftElbowJoint.position.position.X;
		double right_y_diff_2 = leftHandJoint.position.position.Y
			- leftElbowJoint.position.position.Y;
		userInputOutput.rightAngle[1] = atan2(left_y_diff_1, left_x_diff_1);	
    }

	previous_time_point = now;
    // TODO
#endif
}

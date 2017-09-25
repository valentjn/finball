#include "UserInput.hpp"
#include <iostream>
#include <algorithm>

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
UserInput::UserInput(){

#ifndef WITHOUT_KINECT_LIBRARIES
    // init kinect context etc
    context = make_unique<Context>();
    userGenerator = make_unique<UserGenerator>();
    depthGenerator = make_unique<DepthGenerator>();
    previous_time_point = std::chrono::high_resolution_clock::now();
	
	nUsers = 0; nPlayers = MAX_USERS; trackedUsers = 0;
	for(int k = 0; k < PLAYERS; k++){
		playerJoined[k] = false;
	}
	for(int k = 0; k < MAX_USERS; k++){
		userIsPlayer[k] = false;
	}

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
    nanoseconds delta = duration_cast<nanoseconds>(now-previous_time_point);

	XnUInt16 tmpNUsers = MAX_USERS;
	userGenerator->GetUsers(users, tmpNUsers);
	nUsers = tmpNUsers;

    for(XnUInt16 i=0; i < nUsers; i++) {
        if(userGenerator->GetSkeletonCap().IsTracking(users[i])==FALSE) {
            continue;
        }

        XnSkeletonJointTransformation leftHandJoint, rightHandJoint, headJoint;
		userGenerator->GetSkeletonCap()
			.GetSkeletonJoint(users[i],XN_SKEL_LEFT_HAND, leftHandJoint);
		userGenerator->GetSkeletonCap()
			.GetSkeletonJoint(users[i],XN_SKEL_RIGHT_HAND, rightHandJoint);
		userGenerator->GetSkeletonCap()
			.GetSkeletonJoint(users[i],XN_SKEL_HEAD, headJoint);

		if(leftHandJoint.position.position.Y - threshold
			> headJoint.position.position.Y
			&& rightHandJoint.position.position.Y - threshold
			> headJoint.position.position.Y) {

			for(XnUInt16 j=0; j < PLAYERS; j++) {
				if(!playerJoined[j] && !userIsPlayer[i]) {
					players[j] = users[i];
					playerJoined[j] = true;
					userIsPlayer[i] = true;
					playerIndices[j] = i;
					if(nPlayers==trackedUsers)
						nPlayers++;
					trackedUsers++;
				}
			}
		}
    }

	for(XnUInt16 k = 0; k < nPlayers; k++) {
		if(playerJoined[k]) {
			if(!userGenerator->GetSkeletonCap().IsTracking(players[k])) {
				playerJoined[k] = false;
				userIsPlayer[playerIndices[k]] = false;
				trackedUsers--;
				continue;
			}

			XnSkeletonJointTransformation leftHandJoint, rightHandJoint,
				leftElbowJoint, rightElbowJoint;
			userGenerator->GetSkeletonCap().GetSkeletonJoint(players[k],
				XN_SKEL_LEFT_HAND, leftHandJoint);
			userGenerator->GetSkeletonCap().GetSkeletonJoint(players[k],
				XN_SKEL_RIGHT_HAND, rightHandJoint);
			userGenerator->GetSkeletonCap().GetSkeletonJoint(players[k],
				XN_SKEL_LEFT_ELBOW, leftElbowJoint);
			userGenerator->GetSkeletonCap().GetSkeletonJoint(players[k],
				XN_SKEL_RIGHT_ELBOW, rightElbowJoint);

			double ldx = leftHandJoint.position.position.X
				- leftElbowJoint.position.position.X;
			double ldy = leftHandJoint.position.position.Y
				- leftElbowJoint.position.position.Y;
			double rdx = leftHandJoint.position.position.X
				- leftElbowJoint.position.position.X;
			double rdy = leftHandJoint.position.position.Y
				- leftElbowJoint.position.position.Y;

			userInputOutput.leftAngle[k] = std::atan2(ldy, ldx) - zero_angle[k];
			userInputOutput.rightAngle[k] = std::atan2(rdy, rdx) + zero_angle[k];

			if(userInputOutput.leftAngle[k] < -M_PI) {
				userInputOutput.leftAngle[k] += 2*M_PI;
			}
			if(userInputOutput.rightAngle[k] > M_PI) {
				userInputOutput.rightAngle[k] -= 2*M_PI;
			}
			double rightPsi = copysign(M_PI, userInputOutput.rightAngle[k])
				- userInputOutput.rightAngle[k];

			if(userInputOutput.leftAngle[k] > max_angle[k]) {
				userInputOutput.leftAngle[k] = max_angle[k]; 
			} else if(userInputOutput.leftAngle[k] < min_angle[k]) {
				userInputOutput.leftAngle[k] = min_angle[k];
			}

			if(rightPsi > max_angle[k]) {
				userInputOutput.rightAngle[k] = M_PI - max_angle[k]; 
			} else if(rightPsi < min_angle[k]) {
				userInputOutput.rightAngle[k] = -M_PI -min_angle[k];
			}

			double ldAngle = userInputOutput.leftAngle[k] - previousLeftAngles[k];
			double rdAngle = userInputOutput.rightAngle[k] - previousRightAngles[k];
			userInputOutput.leftVelocity[k] = (1000000*ldAngle) / delta.count();
			userInputOutput.rightVelocity[k] = (1000000*rdAngle) / delta.count();

		}
	}

	previous_time_point = now;
	for(int k = 0; k < PLAYERS; k++){
		previousLeftAngles[k] = userInputOutput.leftAngle[k];
		previousRightAngles[k] = userInputOutput.rightAngle[k];
	}

	userInputOutput.start = std::all_of(playerJoined, playerJoined+PLAYERS,
		[](bool x){return x;});
	
#endif
}

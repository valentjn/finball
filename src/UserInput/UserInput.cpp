#include "UserInput.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include "../Log.hpp"

#ifndef WITHOUT_KINECT_LIBRARIES

// headers for kinect
#include <XnCppWrapper.h>

using namespace xn;

#endif

#include <stdexcept>
#include <cmath>

using namespace std;
using namespace std::chrono;

#ifndef WITHOUT_KINECT_LIBRARIES
bool UserInput::initializeKinect() {
	if(kinectIsInitialized) return true;
	
    // init kinect context etc
    context = make_unique<Context>();
    userGenerator = make_unique<UserGenerator>();
    depthGenerator = make_unique<DepthGenerator>();
	
	// TODO: handle error codes
    XnStatus errorCode = XN_STATUS_OK;

    errorCode = context->Init();
	if(errorCode != XN_STATUS_OK) {
		Log::info("Can't initialize context for kinect.");
		Log::debug("Error code: %i", errorCode);
		kinectIsInitialized = false;
		return false;
	}

	// directions are mirrored (so right hand is on the right)
	errorCode = context->SetGlobalMirror(true);
	if(errorCode != XN_STATUS_OK) {
		Log::info("Can't initialize context for kinect.");
		Log::debug("Error code: %i", errorCode);
		kinectIsInitialized = false;
		return false;
	}

	// initialize depth generator
    errorCode = context->FindExistingNode(XN_NODE_TYPE_DEPTH, *depthGenerator);
    if(errorCode != XN_STATUS_OK) {
		Log::info("Failed to initialize Depth Generator. Retrying...");
        errorCode = depthGenerator->Create(*context);
		if(errorCode != XN_STATUS_OK) {
			Log::info("Can't initialize depth generator.");
			Log::debug("Error code: %i", errorCode);
			kinectIsInitialized = false;
			return false;
		}
    }

	// initialize user generator
    errorCode = context->FindExistingNode(XN_NODE_TYPE_USER, *userGenerator);
    if(errorCode != XN_STATUS_OK) {
		Log::info("Failed to initialize User Generator. Retrying...");
        errorCode = userGenerator->Create(*context);
		if(errorCode != XN_STATUS_OK) {
			Log::info("Can't initialize user generator.");
			Log::debug("Error code: %i", errorCode);
			kinectIsInitialized = false;
			return false;
		}
    }

    // register callbacks
    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete;

	// register user callbacks
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
	if(errorCode != XN_STATUS_OK) {
		Log::info("Can't register user callbacks.");
		Log::debug("Error code: %i", errorCode);
		kinectIsInitialized = false;
		return false;
	}

	// register calibration callbacks
    errorCode = userGenerator->GetSkeletonCap().RegisterToCalibrationStart(
        [](SkeletonCapability& skeletonCapability, XnUserID nID, void* cookie){
            UserInput* _this = reinterpret_cast<UserInput*>(cookie);
            // calibration start
        }, this, hCalibrationStart);
	if(errorCode != XN_STATUS_OK) {
		Log::info("Can't register calibration start.");
		Log::debug("Error code: %i", errorCode);
		kinectIsInitialized = false;
		return false;
	}

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
	if(errorCode != XN_STATUS_OK) {
		Log::info("Can't register calibration end.");
		Log::debug("Error code: %i", errorCode);
		kinectIsInitialized = false;
		return false;
	}


    // Get Skeleton
    userGenerator->GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    // start generating data
	Log::debug("Start generating input data...");
    errorCode = context->StartGeneratingAll();
	if(errorCode != XN_STATUS_OK) {
		Log::info("Can't generate data.");
		Log::debug("Error code: %i", errorCode);
		kinectIsInitialized = false;
		return false;
	}

	// set flags
	kinectIsInitialized = true;
	return true;
}
#endif

// initialization of user input facilities
UserInput::UserInput(){
	previous_time_point = std::chrono::high_resolution_clock::now();

#ifndef WITHOUT_KINECT_LIBRARIES
	kinectIsInitialized = false;
	nUsers = 0; nPlayers = MAX_USERS; trackedUsers = 0;
	
	for(int k = 0; k < PLAYERS; k++){
		playerJoined[k] = false;
	}
	for(int k = 0; k < MAX_USERS; k++){
		userIsPlayer[k] = false;
	}

	tryInitializingKinect = true;
	initializeKinect();
#endif

	for(int k=0; k < PLAYERS; k++){
		max_angle[k] = 1.5;
		min_angle[k] = -1.5;
		zero_angle[k] = 0;
	}
	
}

#ifndef WITHOUT_KINECT_LIBRARIES
void UserInput::finalizeKinect() {	
	// cleanup kinect context etc
    depthGenerator->Release();
    userGenerator->Release();
    context->Release();

	depthGenerator = nullptr;
	userGenerator = nullptr;
	context = nullptr;

	kinectIsInitialized = false;
}
#endif

// finalization of user input facilities
UserInput::~UserInput() {
#ifndef WITHOUT_KINECT_LIBRARIES
    finalizeKinect();
#endif
}

// process input
void UserInput::getInput(UserInputOutput &userInputOutput) {
	// get timing information
	auto now = high_resolution_clock::now();
	nanoseconds delta = duration_cast<nanoseconds>(now-previous_time_point);

	// SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            userInputOutput.quit = true;
            break;
        }
    }

#ifndef WITHOUT_KINECT_LIBRARIES
	if (!kinectIsInitialized) {
		if(tryInitializingKinect) {
			// try again to initialize the kinect
			finalizeKinect();
			initializeKinect();
		}
	} else {
		// TODO: handle error codes
		XnStatus errorCode = XN_STATUS_OK;

		context->WaitNoneUpdateAll();

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
					userInputOutput.playerIsTracked[k] = false;
					playerJoined[k] = false;
					userIsPlayer[playerIndices[k]] = false;
					trackedUsers--;
					continue;
				}

				userInputOutput.playerIsTracked[k] = true;

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
				double rdx = rightHandJoint.position.position.X
					- rightElbowJoint.position.position.X;
				double rdy = rightHandJoint.position.position.Y
					- rightElbowJoint.position.position.Y;

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

				rightPsi = copysign(M_PI, userInputOutput.rightAngle[k])
					- userInputOutput.rightAngle[k];

				double previousRightPsi = copysign(M_PI, previousRightAngles[k])
					- previousRightAngles[k];
				double ldAngle = userInputOutput.leftAngle[k] - previousLeftAngles[k];
				double rdAngle = rightPsi - previousRightPsi;
				userInputOutput.leftVelocity[k] = (1000000*ldAngle) / delta.count();
				userInputOutput.rightVelocity[k] = (1000000*rdAngle) / delta.count();

			
				if(ldAngle < MIN_DIFFERENCE && ldAngle > -MIN_DIFFERENCE) {
					userInputOutput.leftAngle[k] = previousLeftAngles[k];
					userInputOutput.leftVelocity[k] = 0;
				}
				if(rdAngle < MIN_DIFFERENCE && rdAngle > -MIN_DIFFERENCE) {
					userInputOutput.rightAngle[k] = previousRightAngles[k];
					userInputOutput.rightVelocity[k] = 0;
				}
			
			}
		}

		for(int k = 0; k < PLAYERS; k++){
			previousLeftAngles[k] = userInputOutput.leftAngle[k];
			previousRightAngles[k] = userInputOutput.rightAngle[k];
		}
		userInputOutput.start = std::all_of(playerJoined, playerJoined+PLAYERS,
			[](bool x){return x;});

	}

#endif
	
	previous_time_point = now;
		
}

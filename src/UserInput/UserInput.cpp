#include "UserInput.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include "../Log.hpp"

#ifdef KINECT_LIBS

// headers for kinect
#include <XnCppWrapper.h>

using namespace xn;

#endif

#include <stdexcept>
#include <cmath>

using namespace std;
using namespace std::chrono;

#ifdef KINECT_LIBS
bool UserInput::initializeKinect() {
    if (kinectIsInitialized) return true;

    // init kinect context etc
    context = make_unique<Context>();
    userGenerator = make_unique<UserGenerator>();
    depthGenerator = make_unique<DepthGenerator>();

    // TODO: handle error codes
    XnStatus errorCode = XN_STATUS_OK;

    errorCode = context->Init();
    if (errorCode != XN_STATUS_OK) {
        Log::info("Can't initialize context for kinect.");
        Log::debug("Error code: %i", errorCode);
        kinectIsInitialized = false;
        return false;
    }

    // directions are mirrored (so right hand is on the right)
    errorCode = context->SetGlobalMirror(true);
    if (errorCode != XN_STATUS_OK) {
        Log::info("Can't initialize context for kinect.");
        Log::debug("Error code: %i", errorCode);
        kinectIsInitialized = false;
        return false;
    }

    // initialize depth generator
    errorCode = context->FindExistingNode(XN_NODE_TYPE_DEPTH, *depthGenerator);
    if (errorCode != XN_STATUS_OK) {
        Log::info("Failed to initialize Depth Generator. Retrying...");
        errorCode = depthGenerator->Create(*context);
        if (errorCode != XN_STATUS_OK) {
            Log::info("Can't initialize depth generator.");
            Log::debug("Error code: %i", errorCode);
            kinectIsInitialized = false;
            return false;
        }
    }

    // initialize user generator
    errorCode = context->FindExistingNode(XN_NODE_TYPE_USER, *userGenerator);
    if (errorCode != XN_STATUS_OK) {
        Log::info("Failed to initialize User Generator. Retrying...");
        errorCode = userGenerator->Create(*context);
        if (errorCode != XN_STATUS_OK) {
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
    if (errorCode != XN_STATUS_OK) {
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
    if (errorCode != XN_STATUS_OK) {
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
            if (status == XN_CALIBRATION_STATUS_OK) {
                _this->userGenerator->GetSkeletonCap().StartTracking(nID);
            } else {
                // TODO: handle errors
                _this->userGenerator->GetSkeletonCap()
                    .RequestCalibration(nID, TRUE);
            }
        }, this, hCalibrationComplete);
    if (errorCode != XN_STATUS_OK) {
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
    if (errorCode != XN_STATUS_OK) {
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
UserInput::UserInput(InputSource mUsedInputSource){
    previous_time_point = std::chrono::high_resolution_clock::now();

#ifdef KINECT_LIBS
    kinectIsInitialized = false;
    nUsers = 0; nPlayers = 0; trackedUsers = 0;

    for (int k = 0; k < PLAYERS; k++){
        playerJoined[k] = false;
    }
    for (int k = 0; k < MAX_USERS; k++){
        userIsPlayer[k] = false;
    }

    tryInitializingKinect = true;
    initializeKinect();
#endif

    for (int k=0; k < PLAYERS; k++){
        max_angle[k] = 1.5;
        min_angle[k] = -1.5;
        zero_angle[k] = 0;
        leftDifferenceTooBig[k] = false;
        rightDifferenceTooBig[k] = false;
    }
    usedInputSource = mUsedInputSource;
}

#ifdef KINECT_LIBS
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
#ifdef KINECT_LIBS
    finalizeKinect();
#endif
}

void UserInput::getSDLInput(UserInputOutput &userInputOutput, double delta) {

    SDL_Event event;

    aal = -a0;
    aar = -a0;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {

        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym){
            case SDLK_LEFT:
                leftPressed = true;
                break;
            case SDLK_RIGHT:
                rightPressed = true;
                break;
            case SDLK_ESCAPE:
                userInputOutput.quit = true;
                return;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym){
            case SDLK_LEFT:
                leftPressed = false;
                break;
            case SDLK_RIGHT:
                rightPressed = false;
                break;
            case SDLK_SPACE:
                Log::debug("UserInput: waiting = false");
                waiting = false;
                if (usedInputSource == CHOOSING) {
                    usedInputSource = KEYBOARD;
                }
#ifdef KINECT_LIBS
                tryInitializingKinect = false;
#endif
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            userInputOutput.mouseX = event.motion.x;
            userInputOutput.mouseY = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            userInputOutput.mouseX = event.button.x;
            userInputOutput.mouseY = event.button.y;
            break;
        case SDL_MOUSEBUTTONUP:
            userInputOutput.mouseX = event.button.x;
            userInputOutput.mouseY = event.button.y;
            break;
        }
    }

    // current v and angle:
    if (leftPressed){
        aal = a0;
    } else {
        avl = 0;
    }

    if (rightPressed){
        aar = a0;
    } else {
        avr = 0;
    }

    avl += (aal*delta);
    avr += (aar*delta);
    anl += (avl*delta);
    anr += (avr*delta);


    if (anl > max_angle[0]){
        anl = max_angle[0];
        avl = 0;
    }
    else if (anl < min_angle[0]){
        anl = min_angle[0];
        avl = 0;
    }
    if (anr > max_angle[0]){
        anr = max_angle[0];
        avr = 0;
    }
    else if (anr < min_angle[0]){
        anr = min_angle[0];
        avr = 0;
    }

    if (usedInputSource == KEYBOARD) {
        userInputOutput.leftAngle[0] = anl;
        userInputOutput.rightAngle[0] = anr;
		Log::debug("UserInput angles: %f, %f", anl, anr);
        if (PLAYERS >= 2) { // TODO: What the hell?
            userInputOutput.leftVelocity[1] = avl;
            userInputOutput.rightVelocity[1] = avr;
        }
    }
}

#ifdef KINECT_LIBS
void UserInput::getKinectInput(UserInputOutput &userInputOutput, double delta) {
    if (!kinectIsInitialized) {
        if (tryInitializingKinect) {
            // try again to initialize the kinect
            finalizeKinect();
            initializeKinect();
        }
    } else {

        context->WaitNoneUpdateAll();

        XnUInt16 tmpNUsers = MAX_USERS;
        userGenerator->GetUsers(users, tmpNUsers);
        nUsers = tmpNUsers;

        for (XnUInt16 i=0; i < nUsers; i++) {
            if (userGenerator->GetSkeletonCap().IsTracking(users[i])==FALSE) {
                continue;
            }

            XnSkeletonJointTransformation leftHandJoint, rightHandJoint, headJoint;
            userGenerator->GetSkeletonCap()
                .GetSkeletonJoint(users[i],XN_SKEL_LEFT_HAND, leftHandJoint);
            userGenerator->GetSkeletonCap()
                .GetSkeletonJoint(users[i],XN_SKEL_RIGHT_HAND, rightHandJoint);
            userGenerator->GetSkeletonCap()
                .GetSkeletonJoint(users[i],XN_SKEL_HEAD, headJoint);

            if (leftHandJoint.position.position.Y - threshold
                > headJoint.position.position.Y
                && rightHandJoint.position.position.Y - threshold
                > headJoint.position.position.Y) {

                for (XnUInt16 j=0; j < PLAYERS; j++) {
                    if (!playerJoined[j] && !userIsPlayer[i]) {
                        players[j] = users[i];
                        playerJoined[j] = true;
                        userIsPlayer[i] = true;
                        playerIndices[j] = i;
                        if (nPlayers==trackedUsers)
                            nPlayers++;
                        trackedUsers++;
                    }
                }
            }
        }

        for (XnUInt16 k = 0; k < nPlayers; k++) {
            if (playerJoined[k]) {
                if (!userGenerator->GetSkeletonCap().IsTracking(players[k])) {
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
                userInputOutput.rightAngle[k]
                    = std::atan2(rdy, -rdx) - zero_angle[k];

                if (userInputOutput.leftAngle[k] < -M_PI) {
                    userInputOutput.leftAngle[k] += 2*M_PI;
                }
                if (userInputOutput.rightAngle[k] < -M_PI) {
                    userInputOutput.rightAngle[k] += 2*M_PI;
                }

                if (userInputOutput.leftAngle[k] > max_angle[k]) {
                    userInputOutput.leftAngle[k] = max_angle[k];
                } else if (userInputOutput.leftAngle[k] < min_angle[k]) {
                    userInputOutput.leftAngle[k] = min_angle[k];
                }

                if (userInputOutput.rightAngle[k] > max_angle[k]) {
                    userInputOutput.rightAngle[k] = max_angle[k];
                } else if (userInputOutput.rightAngle[k] < min_angle[k]) {
                    userInputOutput.rightAngle[k] = min_angle[k];
                }

                double ldAngle
                    = userInputOutput.leftAngle[k] - previousLeftAngles[k];
                double rdAngle
					= userInputOutput.rightAngle[k] - previousRightAngles[k];
                userInputOutput.leftVelocity[k] = ldAngle / delta;
                userInputOutput.rightVelocity[k] = rdAngle / delta;

                if (ldAngle < MIN_DIFFERENCE && ldAngle > -MIN_DIFFERENCE) {
                    userInputOutput.leftAngle[k] = previousLeftAngles[k];
                    userInputOutput.leftVelocity[k] = 0;
                }
                if (rdAngle < MIN_DIFFERENCE && rdAngle > -MIN_DIFFERENCE) {
                    userInputOutput.rightAngle[k] = previousRightAngles[k];
                    userInputOutput.rightVelocity[k] = 0;
                }

                // ease input when difference is big
                if (ldAngle > MAX_DIFFERENCE && ldAngle < -MAX_DIFFERENCE) {
                    if (leftDifferenceTooBig[k]) {
                        userInputOutput.leftAngle[k]
                            = previousLeftAngles[k]
                            + BIG_DIFFERENCE_EASING
                            * (userInputOutput.leftAngle[k]-previousLeftAngles[k]);
                        ldAngle = userInputOutput.leftAngle[k]
                            - previousLeftAngles[k];
                        userInputOutput.leftVelocity[k]
                            = ldAngle / delta;
                    } else {
                        userInputOutput.leftAngle[k] = previousLeftAngles[k];
                        userInputOutput.leftVelocity[k] = 0;
                        leftDifferenceTooBig[k] = true;
                    }
                }
                if (rdAngle > MAX_DIFFERENCE && rdAngle < -MAX_DIFFERENCE) {
                    if (rightDifferenceTooBig[k]) {
                        userInputOutput.rightAngle[k]
                            = previousRightAngles[k]
                            + BIG_DIFFERENCE_EASING
                            * (userInputOutput.rightAngle[k]
                                -previousRightAngles[k]);
                        userInputOutput.rightVelocity[k]
                            = rdAngle / delta;
                    } else {
                        userInputOutput.rightAngle[k] = previousRightAngles[k];
                        userInputOutput.rightVelocity[k] = 0;
                        rightDifferenceTooBig[k] = true;
                    }
                }

            }
        }

        waiting = !std::all_of(playerJoined, playerJoined+PLAYERS,
            [](bool x){return x;});
        if (!waiting && usedInputSource == CHOOSING) {
            usedInputSource = KINECT;
        }
    }
}
#endif

void UserInput::getFakeInput(UserInputOutput &userInputOutput, double delta) {
    static double t = 0;
    t += delta * FAKE_SPEED * M_PI;
    if (t >= 2*M_PI) {
        t -= 2*M_PI;
    }
    double a = 0.5*(std::sin(t)+1);

    for (int k=0; k<PLAYERS; k++) {
        double angle = min_angle[k] + (max_angle[k]-min_angle[k])*a;
        userInputOutput.leftAngle[k] = angle;
        userInputOutput.rightAngle[k] = std::copysign(M_PI, angle) - angle;
    }
}

void UserInput::initOutput(UserInputOutput& userInputOutput) {}

// process input
void UserInput::compute(UserInputOutput &userInputOutput) {
    // get timing information
    auto now = high_resolution_clock::now();
    double delta = duration_cast<nanoseconds>(now-previous_time_point).count()
        / 1000000.0;

    switch(usedInputSource){
    case KEYBOARD:
		Log::debug("UserInput: Source: KEYBOARD");
        getSDLInput(userInputOutput, delta);
        break;
#ifdef KINECT_LIBS
    case KINECT:
		Log::debug("UserInput: Source: KINECT");		
        getSDLInput(userInputOutput, delta);
        getKinectInput(userInputOutput, delta);
        break;
#endif
    case CHOOSING:
		Log::debug("UserInput: Source: CHOOSING");
		getSDLInput(userInputOutput, delta);
#ifdef KINECT_LIBS
        getKinectInput(userInputOutput, delta);
#endif
        break;
    case FAKE:
		Log::debug("UserInput: Source: FAKE");
        getSDLInput(userInputOutput, delta);
        getFakeInput(userInputOutput, delta);
        break;
    default:
        Log::error("Unknown input source selected. Something went terribly wrong.");
    }

    previous_time_point = now;
    for (int k = 0; k < PLAYERS; k++){
        previousLeftAngles[k] = userInputOutput.leftAngle[k];
        previousRightAngles[k] = userInputOutput.rightAngle[k];
    }
}

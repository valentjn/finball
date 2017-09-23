#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_

struct UserInputOutput {
    double leftAngle[2];
    double rightAngle[2];
    double leftVelocity[2];
    double rightVelocity[2];

    bool playerIsTracked[2];
    bool playerIsCalibrating[2];
    bool playerIsCalibrated[2];

    bool quit, start;

    UserInputOutput() : quit(false), start(false) {}
};

#endif

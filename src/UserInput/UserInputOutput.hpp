#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_

#include <chrono>

struct UserInputOutput {
    double leftAngle[2];
    double rightAngle[2];
    double leftVelocity[2];
    double rightVelocity[2];

    bool playerIsTracked[2];
    bool playerIsCalibrating[2];
    bool playerIsCalibrated[2];

	double leftFinStartAngle = 0; // start angle of fins without input
	double rightFinStartAngle = 3.141;
	double maxAngle = 1.571;
	double minAngle =0; // or -0.xx for example
	std::chrono::high_resolution_clock::time_point timeBef;
	std::chrono::high_resolution_clock::time_point timeNow; // time of current angle and velocity
	double a0 = 3.141; // set positive angular acceleration constant

	int mouseX; // x position of mouse in pixels
	int mouseY; // y position of mouse in pixels

    bool quit, start;

    UserInputOutput() : quit(false), start(false) {}
};

#endif

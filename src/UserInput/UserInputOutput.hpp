#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_
#include <iostream>

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

	void print() {
		std::cout << (quit?"Q":"") << (start?"S":"");
		for(int k = 0; k<2; k++) {
			std::cout << (playerIsTracked[k]?"T":"")
					  << (playerIsCalibrating[k]?"c":"")
					  << (playerIsCalibrated[k]?"C":"")
					  << " L: " << leftAngle[k] << " | " << leftVelocity[k]
					  << " R: " << rightAngle[k] << " | " << rightVelocity[k]
					  << " ||";
		}
		std::cout << std::endl;
	}
};

#endif

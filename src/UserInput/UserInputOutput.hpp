#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_
#include <iostream>
#include <iomanip>

struct UserInputOutput {
    double leftAngle[2];
    double rightAngle[2];
    double leftVelocity[2];
    double rightVelocity[2];

    bool playerIsTracked[2];

    bool quit, start;

    UserInputOutput() : quit(false), start(false) {}

	void print() {
		std::cout << (quit?"Q":"") << (start?"S":"");
		for(int k = 0; k<2; k++) {
			std::cout << (playerIsTracked[k]?"T":"")
					  << " L: " << std::setw(15) << leftAngle[k]
					  << " | " << std::setw(15) << leftVelocity[k]
					  << " R: " << std::setw(15) << rightAngle[k]
					  << " | " << std::setw(15) << rightVelocity[k]
					  << " ||";
		}
		std::cout << std::endl;
	}
};

#endif

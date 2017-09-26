#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_
#include <iostream>
#include <iomanip>

#include <chrono>

struct UserInputOutput {
	static const int PLAYERS = 1;
	
    double leftAngle[PLAYERS];
    double rightAngle[PLAYERS];
    double leftVelocity[PLAYERS];
    double rightVelocity[PLAYERS];

    bool playerIsTracked[PLAYERS];

	int mouseX; // x position of mouse in pixels
	int mouseY; // y position of mouse in pixels

    bool quit, start;

    UserInputOutput() : quit(false), start(false) {}

	void print() {
		std::cout << (quit?"Q":"") << (start?"S":"");
		for(int k = 0; k<PLAYERS; k++) {
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

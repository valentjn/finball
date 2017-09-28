#ifndef USER_INPUT_OUTPUT_HPP_
#define USER_INPUT_OUTPUT_HPP_
#include <iostream>
#include <iomanip>

#include <chrono>

struct UserInputOutput {
	static constexpr int PLAYERS = 1;

    std::array<double, PLAYERS> leftAngle;
    std::array<double, PLAYERS> rightAngle;
    std::array<double, PLAYERS> leftVelocity;
    std::array<double, PLAYERS> rightVelocity;

    std::array<bool, PLAYERS> playerIsTracked;

	int mouseX; // x position of mouse in pixels
	int mouseY; // y position of mouse in pixels

    bool quit;

    UserInputOutput() : quit(false) {}

	void print() {
		std::cout << (quit?"Q":"");// << (waiting?"W":"");
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

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

	double pressedL = 0; // increasing with duration of left / right arrow pressed
	double pressedR = 0;
	double maxAngle = 1.571;
	double stepSize = maxAngle/20; // increase of angle with every keydown event
	bool fakedata = false;
	

	int mouseX; // x position of mouse in pixels
	int mouseY; // y position of mouse in pixels

    bool quit, start;

    UserInputOutput() : quit(false), start(false) {}
};

#endif

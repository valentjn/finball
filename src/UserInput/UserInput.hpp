#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#include <chrono>

#include <SDL2/SDL.h>

#include "UserInput/UserInputOutput.hpp"

class UserInput {
public:
    void getInput(UserInputOutput &userInputOutput) {

        SDL_Event event;

		bool leftPressed = false;
	  	bool rightPressed = false;

		double aal =0; 
		double aar =0;// angular acceleration
		double avl =0; // angular velocity
		double avr =0;
		double anl =UserInputOutput.leftFinStartAngle; // angle
		double anr =UserInputOutput.rightFinStartAngle;

	    long dt =0;
		UserInputOutput.timeBef =std::chrono::high_resolution_clock::now();
		UserInputOutput.timeNow =std::chrono::high_resolution_clock::now();

        while (SDL_PollEvent(&event)) {

		aal = -UserInputOutput.a0;
		aar = UserInputOutput.a0;
		if(!leftPressed && !rightPressed){
		UserInputOutput.timeBef = UserInputOutput.timeNow;
		UserInputOutput.timeNow = std::chrono::high_resolution_clock::now();	
		}

            switch (event.type) {
			
            case SDL_QUIT:
                quit = true;
                break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
				case SDLK_LEFT:
					leftPressed = true;
					break;
				case SDLK_RIGHT:
					rightPressed = true;
					break;
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
				}
				break;
			case SDL_MOUSEMOTION:
				UserInputOutput.mouseX = event.motion.x;
				UserInputOutput.mouseY = event.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				UserInputOutput.mouseX = event.button.x;
				UserInputOutput.mouseY = event.button.y;
				break;
			case SDL_MOUSEBUTTONUP:
				UserInputOutput.mouseX = event.button.x;
				UserInputOutput.mouseY = event.button.y;
				break;
            }

		// current v and angle:
		if(leftPressed){
		aal = UserInputOutput.a0;}
		else if (!leftPressed){
		avl =0;
		}

		if(rightPressed){
		aar = -UserInputOutput.a0;}	
		else if(!rightPressed){
		avr=0;
		}		

		UserInputOutput.timeBef = UserInputOutput.timeNow;
		UserInputOutput.timeNow = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow-timeBef).count();	

		avl = avl + (aal*dt/1000);
		avr = avr + (aar*dt/1000);
		anl = anl + (avl*dt/1000);
		anr = anr + (avr*dt/1000);


		if(anl >= UserInputOutput.maxAngle){
		anl = UserInputOutput.maxAngle;
		avl =0;
		}
		else if(anl <= UserInputOutput.minAngle){
		anl =UserInputOutput. minAngle;
		avl =0;
		}
		if(anr <= 3.141-UserInputOutput.maxAngle){
		anr = 3.141-UserInputOutput.maxAngle;
		avr =0;
		}
		else if(anr >= 3.141-UserInputOutput.minAngle){
		anr = 3.141-UserInputOutput.minAngle;
		avr =0;
		}

		UserInputOutput.leftAngle[0]=anl;
		UserInputOutput.rightAngle[0]=anr;
		UserInputOutput.leftVelocity[1]=avl;		
		UserInputOutput.rightVelocity[1]=avr;
	}
        userInputOutput.start = true;
}

};

#endif

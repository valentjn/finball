#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_image.h>


int main(int argc, char ** argv)
{

SDL_Init(SDL_INIT_VIDEO);
	SDL_Window * window = SDL_CreateWindow("Presentation", 
	SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, 0);

 SDL_Event event;

    double leftAngle[2];
    double rightAngle[2];
    double leftVelocity[2];
    double rightVelocity[2];
	double pressedL = 0; // increasing with duration of left / right arrow pressed
	double pressedR = 0;
	double maxAngle = 1.571;
	double stepSize = maxAngle/20; // increase of angle with every keydown event
	bool fakedata = false;
	

	int mouseX; // x position of mouse in pixels
	int mouseY; // y position of mouse in pixels

bool quit = false;
while(!quit){
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_LEFT:
						// 1 keypress is pi/40, velocity starts at pi/10
						pressedL = pressedL - stepSize;
						if (pressedL >= -maxAngle) {
							leftAngle[0] = pressedL;
							leftVelocity[0] = 4*pressedL;
							}
						std::cout << "angle: " << leftAngle[0] << " velocity: " << leftVelocity[0] << std::endl;
						break;
					case SDLK_RIGHT:
						// 1 keypress is pi/40, velocity starts at pi/10
						pressedR = pressedR + stepSize;
						if (pressedR <= maxAngle) {
							rightAngle[0] = pressedR-3.141;
							rightVelocity[0] = 4*pressedR;
							}
						std::cout << "angle: " << rightAngle[0] << " velocity: " << rightVelocity[0] << std::endl;
						break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym){
					case SDLK_LEFT:
						// reset pressedL for the next time
						pressedL = 0;
						std::cout << "released" << std::endl;
						break;
					case SDLK_RIGHT:
						// reset pressedR for the next time
						pressedR = 0;
						std::cout << "released" << std::endl;
						break;
				}
				break;

			case SDL_MOUSEMOTION:
				mouseX = event.motion.x;
				mouseY = event.motion.y;
				std::cout << "motion: X " << mouseX << " Y " << mouseY << std::endl;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(event.button.button == SDL_BUTTON_RIGHT)
					quit = true;
				mouseX = event.button.x;
				mouseY = event.button.y;
				std::cout << "buttondown: X " << mouseX << " Y " << mouseY << std::endl;
				break;
			case SDL_MOUSEBUTTONUP:
				mouseX = event.button.x;
				mouseY = event.button.y;
				std::cout << "buttonup: X " << mouseX << " Y " << mouseY << std::endl;
				break;

            }
        }
}
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

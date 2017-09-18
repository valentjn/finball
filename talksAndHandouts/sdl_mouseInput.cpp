// Example for mouse input in SDL

#include <SDL2/SDL.h>

// 5-Point 2D Jacobi stencil
double *jacobiIteration(double grid[], const int xSize, const int ySize);


// Sets the boundary conditions in the calculation domain
double *setBoundaryConditions(double grid[], const int xSize, const int ySize, const double top, const double bottom, 
                                const double left, const double right);


// Returns a color in ARGB format depending on the input value
Uint32 doubleToColor(double value);


// Helper functions that return a color according to the input value
Uint8 red(double value);
Uint8 green(double value);
Uint8 blue(double value);


int main(int argc, char ** argv) {

    // Size of the window and calculation domain
    const int xSize = 600;
    const int ySize = 600;
    
    
    // Initialize SDL with the video subsystem
    SDL_Init(SDL_INIT_VIDEO);
    
    
    // Create a window on the screen
    SDL_Window *window = SDL_CreateWindow("SDL2 2D Jacobi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xSize, ySize, 
                                            SDL_WINDOW_SHOWN);
    
    
    // Create the renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    
    // Create the texture
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, xSize, ySize);


    // Calculation domain for solving the heat equation
    double *grid = new double[xSize * ySize];
    memset(grid, 0, xSize * ySize * sizeof(double)); // Initialize all values of the grid to 0
    
    
    // Set the boundary conditions in the calculation domain
    grid = setBoundaryConditions(grid, xSize, ySize, 1, 0, 1, 0);


    // Flag whether the left mouse button is pressed
    bool leftMouseButtonDown = false;
    
    
    // Flag whether the user initiated quitting the program
    bool quit = false;
    
    
    // SDL event
    SDL_Event event;
    
    
    // Memory region that contains the pixels that are displayed
    Uint32 *pixels;
    
    
    // Width of a line of pixels in bytes
    int pitch;


    // Game loop: this is the main loop of any SDL program
    while (!quit) {
        //const Uint64 start = SDL_GetPerformanceCounter();
        
        // Update grid doing one Jacobi iteration
        grid = jacobiIteration(grid, xSize, ySize);
        
        // Lock the texture for write-only access
        SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch); 

        // Update the color of each pixel according to the new value
        for (int i = 0; i != ySize; ++i) {
            for (int j = 0; j != xSize; ++j) {
                pixels[i * xSize + j] = doubleToColor(grid[i * xSize + j]);
            }
        }
        
        // Unlock the texture after writing
        SDL_UnlockTexture(texture);
        
        // Update the texture with the new pixel data
        SDL_UpdateTexture(texture, NULL, pixels, xSize * sizeof(Uint32));
        
        // Copy the texture to the renderer
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        
        // Move the contents of the renderer from the back buffer to the front buffer
        SDL_RenderPresent(renderer);
        
        // Check for SDL events
        SDL_PollEvent(&event);
        
        switch (event.type)
        {
            case SDL_QUIT: // User hit the "x" in the corner of the window
                quit = true;
                break;
                
            case SDL_MOUSEBUTTONUP: // User released the left mouse button
                if (event.button.button == SDL_BUTTON_LEFT)
                    leftMouseButtonDown = false;
                break;
                
            case SDL_MOUSEBUTTONDOWN: // User pushed the left mouse button
                if (event.button.button == SDL_BUTTON_LEFT)
                    leftMouseButtonDown = true;
                    
            case SDL_MOUSEMOTION: // User is moving the mouse
                if (leftMouseButtonDown)
                {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;
                    
                    // Include a new 'boundary condition' when the left mouse button is pressed and the mouse is being moved
                    grid[mouseY * xSize + mouseX] = 1;
                }
                break;
        }
        
        //const Uint64 end = SDL_GetPerformanceCounter();
        //const static Uint64 freq = SDL_GetPerformanceFrequency();
        //const double seconds = ( end - start ) / static_cast< double >( freq );
        //std::cout << "Frame time: " << seconds * 1000.0 << "ms" << std::endl;
    }
    
    // Delete the dynamically allocated array
    delete [] grid;
    
    // Free the texture and renderer
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    
    // Free the window
    SDL_DestroyWindow(window);
    
    // Quit SDL
    SDL_Quit();
    
    return 0;
}



double *setBoundaryConditions(double grid[], const int xSize, const int ySize, const double top, const double bottom, 
                                const double left, const double right) {
  
    // Set boundary conditions at the left and right edges
    for (int i = 0; i != ySize; ++i) {
        grid[i * xSize] = left;
        grid[i * xSize + xSize] = right;
    }
        
    // Set boundary conditions at the top and bottom edges
    for (int i = 0; i != xSize; ++i) {
        grid[i] = top;
        grid[i + xSize * (ySize - 1)] = bottom;
    }
    
    return grid;
}


double *jacobiIteration(double grid[], const int xSize, const int ySize) {

    double gridTmp[xSize * ySize];
    for (int i = 0; i != xSize * ySize; ++i) { 
        gridTmp[i] = grid[i];
    }
    
    for(int i = 1; i != ySize - 1; ++i) {
        for(int j = 1; j != xSize - 1; ++j) {
        
            if (grid[i * xSize + j] != 1) {
                
                grid[i * xSize + j] = 0.25 * (gridTmp[(i - 1) * xSize + j] + gridTmp[(i + 1) * xSize + j] + 
                                        gridTmp[i * xSize + j - 1] + gridTmp[i * xSize + j + 1]);
            }
        }
    }
    
    return grid;
}


Uint32 doubleToColor(double value)
{
    Uint8 alpha = 0;
    Uint8 r = red(value);
    Uint8 g = green(value);
    Uint8 b = blue(value);
    
    return b | g << 8 | r << 16 | alpha << 24;
}


Uint8 red(double value) {
	int r;
	if(value < 0.25)
		r = 255;
	else
		if(value > 0.5)
			r = 0;
		else
			r = -1020 * (value - 0.25) + 255;
	return r;
}


Uint8 green(double value) {
	int g;
	if(value < 0.25)
		g = 1020 * value;
	else
		if(value > 0.75)
			g = -1020 * (value - 0.75) + 255;
		else
			g = 255;
	return g;
}


Uint8 blue(double value) {
	int b;
	if(value < 0.5)
		b = 0;
	else
		if(value > 0.75)
			b = 255;
		else
			b = 1020 * (value - 0.5);
	return b;
}

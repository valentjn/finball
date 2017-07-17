#ifndef CSDLINTERFACE_HPP_
#define CSDLINTERFACE_HPP_

#include <iostream>
#include <string>
#include <signal.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#include "GL/gl.h"

/**
 * sdl inteface class to create OpenGL window and event callbacks
 *
 * http://www.libsdl.org/docs/html/guidevideoopengl.html
 */
class CSDLInterface
{
public:
	/* Dimensions of our window. */
	int width;
	int height;

private:
	/* SDL Window object */
	SDL_Window *window;

	/* Color depth in bits of our window. */
	int bpp;

	/* Title of the window */
	std::string window_title;

	/* mouse position */
	int mouse_x;
	int mouse_y;

	Uint32 prev_fps_update_ticks;
	int buffer_swaps_since_last_fps_update;

	/* bit flag for pressed mouse buttons */
	unsigned int mouse_buttons;

	void create_window()
	{
		int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;// | SDL_FULLSCREEN;
		/*
		 * Set the video mode
		 */
		window = SDL_CreateWindow( window_title.c_str(), SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, width, height, flags );
		if(window == nullptr)
		{
			/*
			 * This could happen for a variety of reasons,
			 * including DISPLAY not being set, the specified
			 * resolution not being available, etc.
			 */
			std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
			exit(-1);
		}

		/*
		 * Create OpenGL context
		 */
		if(SDL_GL_CreateContext(window) == nullptr)
		{
			std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
			exit(-1);
		}
	}

	void resize(int i_width, int i_height)
	{
		width = i_width;
		height = i_height;

	    /* Update our viewport. */
	    glViewport(0, 0, width, height);
	}

	void setup()
	{
		/* Information about the current video settings. */
		SDL_DisplayMode mode;


		/* First, initialize SDL's video subsystem. */
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
			/* Failed, exit. */
			std::cerr << "Video initialization failed: " << SDL_GetError() << std::endl;
			exit(-1);
		}

		/* restore signal handler to allow ctrl-c */
		signal(SIGINT, SIG_DFL);

		/* Let's get the current display mode. */
		if(SDL_GetCurrentDisplayMode(0, &mode) != 0)
		{
			/* This should probably never happen. */
			std::cerr << "Video query failed: " << SDL_GetError() << std::endl;
			exit(-1);
		}

		/*
		 * Set our width/height to 640/480 (you would
		 * of course let the user decide this in a normal
		 * app). We get the bpp we will request from
		 * the display. On X11, VidMode can't change
		 * resolution, so this is probably being overly
		 * safe. Under Win32, ChangeDisplaySettings
		 * can change the bpp.
		 */
		width = 640;
		height = 480;
		bpp = SDL_BITSPERPIXEL(mode.format);
		window_title = "My Window Title";

		/*
		 * Now, we want to setup our requested
		 * window attributes for our OpenGL window.
		 * We want *at least* 5 bits of red, green
		 * and blue. We also want at least a 16-bit
		 * depth buffer.
		 *
		 * The last thing we do is request a double
		 * buffered window. '1' turns on double
		 * buffering, '0' turns it off.
		 *
		 * Note that we do not use SDL_DOUBLEBUF in
		 * the flags to SDL_SetVideoMode. That does
		 * not affect the GL attribute state, only
		 * the standard 2D blitting setup.
		 */
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		/*
		 * We want to request that SDL provide us
		 * with an OpenGL window, in a fullscreen
		 * video mode.
		 *
		 * EXERCISE:
		 * Make starting windowed an option, and
		 * handle the resize events properly with
		 * glViewport.
		 */

		create_window();

		/*********** OPEN GL SETUP ************/


	    // resize viewport
	    resize(width, height);

	    /* Culling. */
	    glCullFace(GL_BACK);
	    glFrontFace(GL_CCW);
	    glEnable(GL_CULL_FACE);

	    /* Set the clear color. */
	    glClearColor(0, 0, 0, 0);
    }


	void shutdown()
	{
		SDL_Quit();
	}


public:
	CSDLInterface()	:
		bpp(0),
		mouse_x(-1),
		mouse_y(-1),
		prev_fps_update_ticks(0),
		buffer_swaps_since_last_fps_update(0),
		mouse_buttons(0)
	{
		setup();
	}


	virtual ~CSDLInterface()
	{
		shutdown();
	}


	virtual void sdl_handle_key_down(
			SDL_Keysym &i_keysym
	) = 0;

	virtual void sdl_handle_key_up(
			SDL_Keysym &i_keysym
	) = 0;

	virtual void sdl_handle_mouse_button_down(
			int i_button_id,
			int i_mouse_buttons,
			int mouse_x,
			int mouse_y
	) = 0;

	virtual void sdl_handle_mouse_button_up(
			int i_button_id,
			int i_mouse_buttons,
			int mouse_x,
			int mouse_y
	) = 0;

	virtual void sdl_handle_mouse_motion(
			int i_mouse_buttons,
			int mouse_x,
			int mouse_y
	) = 0;

	virtual void sdl_handle_quit() = 0;


	void sdl_process_events()
	{
		/* Our SDL event placeholder. */
		SDL_Event event;

		/* Grab all the events off the queue. */
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					/* Handle key presses. */
					sdl_handle_key_down(event.key.keysym);
					break;

				case SDL_WINDOWEVENT_RESIZED:
					resize(event.window.data1, event.window.data2);
					create_window();
					break;

				case SDL_MOUSEBUTTONDOWN:
					mouse_buttons |= (1 << event.button.button);
					sdl_handle_mouse_button_down(event.button.button, mouse_buttons, mouse_x, mouse_y);
					break;

				case SDL_MOUSEBUTTONUP:
					mouse_buttons &= ~(1 << event.button.button);
					sdl_handle_mouse_button_up(event.button.button, mouse_buttons, mouse_x, mouse_y);
					break;

				case SDL_MOUSEMOTION:
					mouse_x = event.motion.x;
					mouse_y = event.motion.y;
					sdl_handle_mouse_motion(mouse_buttons, mouse_x, mouse_y);
					break;

				case SDL_QUIT:
					sdl_handle_quit();
					break;
			}
		}
	}

	void sdl_swap_buffers()
	{
		SDL_GL_SwapWindow(window);

		buffer_swaps_since_last_fps_update++;

		Uint32 current_ticks = SDL_GetTicks();
		Uint32 delta_ticks = current_ticks - prev_fps_update_ticks;

		if (delta_ticks >= 1000)
		{
			double fps = (double)buffer_swaps_since_last_fps_update/((double)delta_ticks*0.001);
			char buffer[1024];
			sprintf(buffer, "FPS: %f", fps);

			buffer_swaps_since_last_fps_update = 0;
			prev_fps_update_ticks += 1000;

			if (prev_fps_update_ticks+1000 < current_ticks)
				prev_fps_update_ticks = current_ticks;
		}
	}
};

#endif /* CSDLINTERFACE_HPP_ */

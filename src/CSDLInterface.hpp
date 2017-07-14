#ifndef CSDLINTERFACE_HPP_
#define CSDLINTERFACE_HPP_

#include <iostream>
#include <signal.h>
#include "SDL.h"
#include "SDL/SDL_video.h"
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
	/* Color depth in bits of our window. */
	int bpp;

	/* mouse position */
	int mouse_x;
	int mouse_y;

	Uint32 prev_fps_update_ticks;
	int buffer_swaps_since_last_fps_update;

	/* bit flag for pressed mouse buttons */
	unsigned int mouse_buttons;

	void update_videomode()
	{
		int flags = SDL_OPENGL | SDL_RESIZABLE | SDL_DOUBLEBUF | SDL_HWSURFACE;// | SDL_FULLSCREEN;
		/*
		 * Set the video mode
		 */
		if( SDL_SetVideoMode( width, height, bpp, flags ) == 0 )
		{
			/*
			 * This could happen for a variety of reasons,
			 * including DISPLAY not being set, the specified
			 * resolution not being available, etc.
			 */
			std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
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
		const SDL_VideoInfo* info = 0;


		/* First, initialize SDL's video subsystem. */
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
			/* Failed, exit. */
			std::cerr << "Video initialization failed: " << SDL_GetError() << std::endl;
			exit(-1);
		}

		/* restore signal handler to allow ctrl-c */
		signal(SIGINT, SIG_DFL);

		/* Let's get some video information. */
		info = SDL_GetVideoInfo();

		if(!info)
		{
			/* This should probably never happen. */
			std::cerr << "Video query failed: " << SDL_GetError() << std::endl;
			exit(-1);
		}

		// enable key repeat
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

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
		bpp = info->vfmt->BitsPerPixel;

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

		update_videomode();

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
			SDL_keysym &i_keysym
	) = 0;

	virtual void sdl_handle_key_up(
			SDL_keysym &i_keysym
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

				case SDL_VIDEORESIZE:
					resize(event.resize.w, event.resize.h);
					update_videomode();
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
		SDL_GL_SwapBuffers( );

		buffer_swaps_since_last_fps_update++;

		Uint32 current_ticks = SDL_GetTicks();
		Uint32 delta_ticks = current_ticks - prev_fps_update_ticks;

		if (delta_ticks >= 1000)
		{
			double fps = (double)buffer_swaps_since_last_fps_update/((double)delta_ticks*0.001);
			char buffer[1024];
			sprintf(buffer, "FPS: %f", fps);

			SDL_WM_SetCaption(buffer, 0);
			buffer_swaps_since_last_fps_update = 0;
			prev_fps_update_ticks += 1000;

			if (prev_fps_update_ticks+1000 < current_ticks)
				prev_fps_update_ticks = current_ticks;
		}
	}
};

#endif /* CSDLINTERFACE_HPP_ */

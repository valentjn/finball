#ifndef CSTAGE_VIDEOOUTPUT_HPP_
#define CSTAGE_VIDEOOUTPUT_HPP_

#include "CPipelineStage.hpp"
#include "CParameters.hpp"
#include "CDataArray2D.hpp"
#include "CSDLInterface.hpp"
#include "CGlTexture.hpp"

class CStage_VideoOutput	:
	public CPipelineStage,
	public CSDLInterface
{
	CParameters &cParameters;

	CDataArray2D<unsigned char,3> output_cDataArray2D_uc3;
	CDataArray2D<float,2> output_cDataArray2D_f2;
	CDataArray2D<float,3> output_cDataArray2D_f3;

	CDataArray2D<unsigned char,4> output_cDataArray2D_uc4_flags;
	CGlTexture cGlTextureFlags;

	/**
	 * OpenGL handler to texture
	 */
	CGlTexture cGlTexture;

	/**
	 * update the texture
	 */
	bool updateTexture;


public:
	CStage_VideoOutput(CParameters &i_cParameters)	:
		CPipelineStage("ImageInput"),
		cParameters(i_cParameters),
		cGlTexture(GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE),
		updateTexture(false)
	{
	}


	virtual ~CStage_VideoOutput()
	{
	}


	/**
	 * incoming data to process from pipeline
	 */
	void pipeline_process_input(
			CPipelinePacket &i_cPipelinePacket
	)
	{

		output_cDataArray2D_uc3.cleanup();
		output_cDataArray2D_f2.cleanup();
		output_cDataArray2D_f3.cleanup();

		/**
		 * RGB image
		 */
		if (i_cPipelinePacket.type_info_name == typeid(CDataArray2D<unsigned char,3>).name())
		{
			CDataArray2D<unsigned char,3> *input = i_cPipelinePacket.getPayload<CDataArray2D<unsigned char,3> >();

			output_cDataArray2D_uc3.resize(input->width, input->height);
			output_cDataArray2D_uc3.loadData(input->data);

			updateTexture = true;
			return;
		}

		/**
		 * velocity field
		 */
		if (i_cPipelinePacket.type_info_name == typeid(CDataArray2D<float,2>).name())
		{
			CDataArray2D<float,2> *input = i_cPipelinePacket.getPayload<CDataArray2D<float,2> >();

			output_cDataArray2D_f2.resize(input->width, input->height);
			output_cDataArray2D_f2.loadData(input->data);

			updateTexture = true;
			return;
		}

		/**
		 * velocity field
		 */
		if (i_cPipelinePacket.type_info_name == typeid(CDataArray2D<float,3>).name())
		{
			CDataArray2D<float,3> *input = i_cPipelinePacket.getPayload<CDataArray2D<float,3> >();

			output_cDataArray2D_f3.resize(input->width, input->height);
			output_cDataArray2D_f3.loadData(input->data);

			updateTexture = true;
			return;
		}

		/**
		 * flag field
		 */
		if (i_cPipelinePacket.type_info_name == typeid(CDataArray2D<unsigned char,1>).name())
		{
			CDataArray2D<unsigned char,1> *input = i_cPipelinePacket.getPayload<CDataArray2D<unsigned char,1> >();

			setupFlagField(input);
			return;
		}

		std::cerr << "ERROR: Video Output is only able to process (char,3) and (float,2) arrays, got " << i_cPipelinePacket.type_info_name << std::endl;
		exit(-1);
	}



	/**
	 * callback from mainloop
	 */
	void main_loop_callback()
	{
		sdl_process_events();

		draw_scene();

		sdl_swap_buffers();
	}

	/**
	 * setup the flag field for visualization output based on flag input field
	 */
	void setupFlagField(
		CDataArray2D<unsigned char,1> *i_input
	)
	{
		output_cDataArray2D_uc4_flags.resize(i_input->width, i_input->height);

		for (int y = 0; y < output_cDataArray2D_uc4_flags.height; y++)
		{
			for (int x = 0; x < output_cDataArray2D_uc4_flags.width; x++)
			{
				unsigned char *d = &output_cDataArray2D_uc4_flags.getRef(x,y,0);

				switch(i_input->getRef(x, y))
				{
				case 0:	// fluid
					d[0] = 255;
					d[1] = 255;
					d[2] = 255;
					d[3] = 0;
					break;

				case 1:	// obstacle
					d[0] = 0;
					d[1] = 0;
					d[2] = 0;
					d[3] = 255;
					break;

				case 2:	// inflow
					d[0] = 255;
					d[1] = 0;
					d[2] = 0;
					d[3] = 255;
					break;

				case 3:	// outflow
					d[0] = 0;
					d[1] = 0;
					d[2] = 255;
					d[3] = 255;
					break;

				default:
					std::cout << "Unknown flag " << i_input->getRef(x, y) << std::endl;
					break;
				}
			}
		}


		cGlTextureFlags.bind();

			if (cGlTextureFlags.width != i_input->width || cGlTextureFlags.height != i_input->height)
				cGlTextureFlags.resize(i_input->width, i_input->height);

			cGlTextureFlags.setTextureParameters(
					GL_TEXTURE_2D,
					GL_RGBA,		// internal format
					GL_RGBA,
					GL_UNSIGNED_BYTE
				);

			cGlTextureFlags.setData(output_cDataArray2D_uc4_flags.data);

		cGlTextureFlags.unbind();
	}

public:
	void postprocess()
	{
		if (!cParameters.stage_fluidsimulation_visualize_flagfield)
			return;

		if (!output_cDataArray2D_uc4_flags.isValidData())
			return;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		cGlTextureFlags.bind();

			glBegin(GL_QUADS);

				glTexCoord2f(0, 1);		glVertex2f(-1, -1);
				glTexCoord2f(1, 1);		glVertex2f(1, -1);
				glTexCoord2f(1, 0);		glVertex2f(1, 1);
				glTexCoord2f(0, 0);		glVertex2f(-1, 1);

			glEnd();

		cGlTextureFlags.unbind();
		glDisable(GL_BLEND);
	}

private:
	/**
	 * draw the scene to screen
	 */
	void draw_scene()
	{
		glClear(GL_COLOR_BUFFER_BIT);

	    /*
	     * setup projection matrix to orthogonal
	     */
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    glOrtho(-1, 1, -1, 1, -1, 1);


	    /*
	     * setup model-view matrix to identity
	     */
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();

	    glEnable(GL_TEXTURE_2D);

		cGlTexture.bind();

			if (updateTexture)
			{
				if (output_cDataArray2D_uc3.isValidData())
				{
					cGlTexture.setTextureParameters(
							GL_TEXTURE_2D,
							GL_RGB,		// internal format
							GL_RGB,
							GL_UNSIGNED_BYTE
						);

					if (cGlTexture.width != output_cDataArray2D_uc3.width || cGlTexture.height != output_cDataArray2D_uc3.height)
						cGlTexture.resize(output_cDataArray2D_uc3.width, output_cDataArray2D_uc3.height);

					cGlTexture.setData(output_cDataArray2D_uc3.data);
				}
				else if (output_cDataArray2D_f2.isValidData())
				{
					cGlTexture.setTextureParameters(
							GL_TEXTURE_2D,
							GL_RG32F,		// internal format
							GL_RG,
							GL_FLOAT
						);

					if (cGlTexture.width != output_cDataArray2D_f2.width || cGlTexture.height != output_cDataArray2D_f2.height)
						cGlTexture.resize(output_cDataArray2D_f2.width, output_cDataArray2D_f2.height);

					cGlTexture.setData(output_cDataArray2D_f2.data);
				}
				else if (output_cDataArray2D_f3.isValidData())
				{
					cGlTexture.setTextureParameters(
							GL_TEXTURE_2D,
							GL_RGB32F,		// internal format
							GL_RGB,
							GL_FLOAT
						);

					if (cGlTexture.width != output_cDataArray2D_f3.width || cGlTexture.height != output_cDataArray2D_f3.height)
						cGlTexture.resize(output_cDataArray2D_f3.width, output_cDataArray2D_f3.height);

					cGlTexture.setData(output_cDataArray2D_f3.data);
				}
				else
				{
					std::cout << "nothing to draw" << std::endl;
					cGlTexture.unbind();
					return;
				}

				updateTexture = false;
			}


			glBegin(GL_QUADS);

				glTexCoord2f(0, 1);		glVertex2f(-1, -1);
				glTexCoord2f(1, 1);		glVertex2f(1, -1);
				glTexCoord2f(1, 0);		glVertex2f(1, 1);
				glTexCoord2f(0, 0);		glVertex2f(-1, 1);

			glEnd();

		cGlTexture.unbind();


		postprocess();
	}

public:
	/**
	 * callback from SDL for key down event
	 */
	void sdl_handle_key_down(
			SDL_Keysym &i_keysym
	)
	{
		char key;
		switch(i_keysym.sym)
		{
		case SDLK_q:
			cParameters.exit = true;
			break;

		default:
			key = i_keysym.sym;
			cParameters.key_down(key);
		}
	}


	void sdl_handle_key_up(
			SDL_Keysym &i_keysym
	)
	{
		switch(i_keysym.sym)
		{
		default:
			break;
		}
	}


	inline void pipeline_push_drawing(
			int i_mouse_x,
			int i_mouse_y,
			unsigned char r,
			unsigned char g,
			unsigned char b
	)
	{
		CDataDrawingInformation d;
		d.x = (double)i_mouse_x/(double)width;
		d.y = (double)i_mouse_y/(double)height;
		d.size = 0.1;
		d.color.r = r;
		d.color.g = g;
		d.color.b = b;

		pipeline_push(d);
	}

	void interactiveStuff(
			int i_mouse_buttons,
			int i_mouse_x,
			int i_mouse_y
	)
	{
		// left+right
		if (i_mouse_buttons == 10)
		{
			// clear
			pipeline_push_drawing(i_mouse_x, i_mouse_y, 255, 255, 255);
			return;
		}

		// left
		if (i_mouse_buttons & 2)
			pipeline_push_drawing(i_mouse_x, i_mouse_y, 255, 0, 0);

		// right
		if (i_mouse_buttons & 8)
			pipeline_push_drawing(i_mouse_x, i_mouse_y, 0, 0, 255);

		// middle
		if (i_mouse_buttons & 4)
			pipeline_push_drawing(i_mouse_x, i_mouse_y, 0, 0, 0);
	}

	void sdl_handle_mouse_button_down(
			int i_button_id,
			int i_mouse_buttons,
			int i_mouse_x,
			int i_mouse_y
	)
	{
		(void)i_button_id;
		interactiveStuff(i_mouse_buttons, i_mouse_x, i_mouse_y);
	}


	void sdl_handle_mouse_button_up(
			int i_button_id,
			int i_mouse_buttons,
			int i_mouse_x,
			int i_mouse_y
	)
	{
		(void)i_button_id;
		interactiveStuff(i_mouse_buttons, i_mouse_x, i_mouse_y);
	}


	void sdl_handle_mouse_motion(
			int i_mouse_buttons,
			int i_mouse_x,
			int i_mouse_y
	)
	{
		interactiveStuff(i_mouse_buttons, i_mouse_x, i_mouse_y);
	}

	virtual void sdl_handle_quit()
	{
		cParameters.exit = true;
	}
};

#endif /* CSTAGE_VIDEOOUTPUT_HPP_ */

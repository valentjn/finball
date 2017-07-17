#ifndef CSTAGE_IMAGEINPUT_HPP_
#define CSTAGE_IMAGEINPUT_HPP_

#include "CParameters.hpp"
#include "CPipelineStage.hpp"
#include "SDL2/SDL_image.h"
#include "CDataArray2D.hpp"
#include "CDataDrawingInformation.hpp"

/**
 * class providing static image input
 *
 * the image is send to the pipeline during each main loop iteration
 */
class CStage_ImageInput	:	public
	CPipelineStage
{
	/**
	 * global parameters
	 */
	CParameters &cParameters;

	/**
	 * storage of image and packet for pipelining
	 */
	CDataArray2D<unsigned char,3> cDataArray2D;

private:
	/**
	 * load the images stored at i_path and save data to cDataArray2D
	 */
	void load_image(std::string &i_path)
	{
		SDL_Surface *image = IMG_Load(i_path.c_str());
		if ( !image )
		{
			std::cerr << "IMG_Load: " << IMG_GetError();
			exit(-1);
		}

		if (image->format->BitsPerPixel != 24)
		{
			std::cerr << "only 24 bpp supported for image input" << std::endl;
			exit(-1);
		}

		cDataArray2D.resize(image->w, image->h);
		cDataArray2D.loadData(image->pixels);

		SDL_FreeSurface(image);
	}


public:
	/**
	 * constructor
	 */
	CStage_ImageInput(CParameters &i_cParameters)	:
		CPipelineStage("ImageInput"),
		cParameters(i_cParameters)
	{
		load_image(cParameters.stage_image_input_path);
	}


public:
	/**
	 * manually triggered pushing of next image to the pipeline
	 */
	void pipeline_push()
	{
		CPipelineStage::pipeline_push((CPipelinePacket&)cDataArray2D);
	}



private:
	/**
	 * draw a circle into an image
	 */
	void draw_circle(
			CDataDrawingInformation &i_d
	)
	{
		int i_x = i_d.x*(double)cDataArray2D.width;
		int i_y = i_d.y*(double)cDataArray2D.height;

		int i_size = i_d.size*(double)cDataArray2D.width;

		for (int ry = -i_size; ry < i_size; ry++)
		{
			int y = i_y + ry;

			if (y < 0 || y >= cDataArray2D.height)
					continue;

			for (int rx = -i_size; rx < i_size; rx++)
			{
				if (rx*rx + ry*ry > i_size)
					continue;

				int x = i_x + rx;

				if (x < 0 || x >= cDataArray2D.width)
						continue;

				unsigned char *d = &cDataArray2D.getRef(x, y);
				d[0] = i_d.color.r;
				d[1] = i_d.color.g;
				d[2] = i_d.color.b;
			}
		}
	}

public:
	/**
	 * process incoming pipeline input.
	 *
	 * the only input processed so far is from the video output stage to
	 * draw something into the image.
	 */
	void pipeline_process_input(
			CPipelinePacket &i_cPipelinePacket
	)
	{
		if (i_cPipelinePacket.type_info_name != typeid(CDataDrawingInformation).name())
		{
			std::cerr << "ERROR: Only data drawing information allowed for image input stage" << std::endl;
			exit(-1);
		}

		// unpack
		CDataDrawingInformation *d = i_cPipelinePacket.getPayload<CDataDrawingInformation>();

		// handle package by drawing a circle
		draw_circle(*d);

		// push modifications on image to pipeline
		pipeline_push();
	}
};

#endif /* CSTAGE_IMAGEINPUT_HPP_ */

#ifndef CSTAGE_VIDEOINPUT_HPP_
#define CSTAGE_VIDEOINPUT_HPP_

#include "CParameters.hpp"
#include "CPipelineStage.hpp"
#include "SDL/SDL_image.h"
#include "CDataArray2D.hpp"
#include "CDataDrawingInformation.hpp"
#include "v4l/imageReader.hpp"
#include <cassert>

/**
 * class providing static image input
 *
 * the image is send to the pipeline during each main loop iteration
 */
class CStage_VideoInput	:	public
	CPipelineStage,
	ImageReader
{
	/**
	 * global parameters
	 */
	CParameters &cParameters;

	/**
	 * storage of image and packet for pipelining
	 */
	CDataArray2D<unsigned char,3> cDataArray2D;


public:
	/**
	 * constructor
	 */
	CStage_VideoInput(CParameters &i_cParameters)	:
		CPipelineStage("ImageInput"),
		ImageReader(	i_cParameters.video_device,
						IO_METHOD_MMAP,	// io method
						1,	// force format
						0	// frame number
		),
		cParameters(i_cParameters)
	{
		setVideoInputSize(cParameters.input_video_width, cParameters.input_video_height);

		std::cout << "V4L: Opening video device " << i_cParameters.video_device << std::endl;
		open_device();

		std::cout << "V4L: Initializing device" << std::endl;
		init_device();

		if (cParameters.verbosity_level > 5)
			std::cout << "Using input video size of " << image_width << " x " << image_height << std::endl;

		// dummy data: blue color
		cDataArray2D.resize(image_width, image_height);
		unsigned char *d = cDataArray2D.data;
		for (int i = 0; i < image_width*image_height*3; i++)
			d[i] = 100;

		std::cout << "V4L: Starting capturing" << std::endl;
		start_capturing();

	}

	void main_loop_callback()
	{
		if (is_input_available())
			read_frame();
	}

	virtual ~CStage_VideoInput()
	{
		std::cout << "V4L: Stopping capturing" << std::endl;
		stop_capturing();

		std::cout << "V4L: Shutdown device" << std::endl;
	    uninit_device();

		std::cout << "V4L: Close device" << std::endl;
		close_device();
	}


private:
	/**
	 * callback handler from imageReader
	 */
    void processImage(
    		const void* p,
    		int i_size
    )
    {
    	assert(image_width*image_height*3 == i_size);
    	memcpy(cDataArray2D.data, p, image_width*image_height*3);

    	pipeline_push();
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

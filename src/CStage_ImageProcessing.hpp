#ifndef CSTAGE_IMAGEPROCESSING_HPP_
#define CSTAGE_IMAGEPROCESSING_HPP_

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
class CStage_ImageProcessing	:	public
	CPipelineStage
{
	/**
	 * global parameters
	 */
	CParameters &cParameters;

	/**
	 * input image
	 */
	CDataArray2D<unsigned char,3> input_cDataArray2D;

	/**
	 * processed image
	 */
	CDataArray2D<unsigned char,3> output_cDataArray2D_uc3;

	/**
	 * processed flagfield
	 */
	CDataArray2D<unsigned char,1> output_cDataArray2D_uc1;

public:
	/**
	 * constructor
	 */
	CStage_ImageProcessing(CParameters &i_cParameters)	:
		CPipelineStage("ImageProcessing"),
		cParameters(i_cParameters)
	{
	}


public:
	/**
	 * manually triggered pushing of next image to the pipeline
	 */
	void pipeline_push()
	{
		if (cParameters.stage_imageprocessing_output_flagfield)
			CPipelineStage::pipeline_push((CPipelinePacket&)output_cDataArray2D_uc1);
		else
			CPipelineStage::pipeline_push((CPipelinePacket&)output_cDataArray2D_uc3);
	}


public:
	/**
	 * apply some filter
	 */
	void apply_filter_blur()
	{
		output_cDataArray2D_uc3.resize(input_cDataArray2D.width, input_cDataArray2D.height);

		int s = 5;
		int inv_s_squared = (s*2+1)*(s*2+1);

		for (int y = s; y < input_cDataArray2D.height-s; y++)
			for (int x = s; x < input_cDataArray2D.width-s; x++)
			{
				int r = 0;
				int g = 0;
				int b = 0;

				for (int ry = -s; ry <= s; ry++)
				{
					for (int rx = -s; rx <= s; rx++)
					{
						r += input_cDataArray2D.getRef(x+rx, y+ry, 0);
						g += input_cDataArray2D.getRef(x+rx, y+ry, 1);
						b += input_cDataArray2D.getRef(x+rx, y+ry, 2);
					}
				}

				output_cDataArray2D_uc3.getRef(x, y, 0) = r/inv_s_squared;
				output_cDataArray2D_uc3.getRef(x, y, 1) = g/inv_s_squared;
				output_cDataArray2D_uc3.getRef(x, y, 2) = b/inv_s_squared;
			}
	}


public:
	void apply_filter_threshold()
	{
		output_cDataArray2D_uc3.resize(input_cDataArray2D.width, input_cDataArray2D.height);

		int threshold = cParameters.stage_imageprocessing_threshold_value;

		for (int y = 0; y < input_cDataArray2D.height; y++)
			for (int x = 0; x < input_cDataArray2D.width; x++)
			{
				unsigned char *d = &input_cDataArray2D.getRef(x, y);

#if 1
				unsigned char r = (d[0] > threshold ? 255 : 0);
				unsigned char g = (d[1] > threshold ? 255 : 0);
				unsigned char b = (d[2] > threshold ? 255 : 0);

#else
				int s = d[0]+d[1]+d[2];

				unsigned char r = (s < threshold*3 ? 0 : 255);
				unsigned char g = r;
				unsigned char b = r;
#endif

				output_cDataArray2D_uc3.getRef(x, y, 0) = r;
				output_cDataArray2D_uc3.getRef(x, y, 1) = g;
				output_cDataArray2D_uc3.getRef(x, y, 2) = b;
			}
	}




public:
	void apply_filter_flagfield()
	{
		output_cDataArray2D_uc1.resize(input_cDataArray2D.width, input_cDataArray2D.height);

		int threshold = cParameters.stage_imageprocessing_threshold_value;

		for (int y = 0; y < input_cDataArray2D.height; y++)
		{
			for (int x = 0; x < input_cDataArray2D.width; x++)
			{
				unsigned char *d = &input_cDataArray2D.getRef(x, y);

				// WHITE
				if (	d[0] >= threshold &&
						d[1] >= threshold &&
						d[2] >= threshold
				)
				{
					// boundary
					output_cDataArray2D_uc1.getRef(x, y, 0) = 0;
					continue;

				}

				// RED
				if (d[0] > threshold)
				{
					// flow input: 1
					output_cDataArray2D_uc1.getRef(x, y, 0) = 2;
					continue;
				}

				// BLUE
				if (d[2] > threshold)
				{
					// flow output: 1
					output_cDataArray2D_uc1.getRef(x, y, 0) = 3;
					continue;
				}

				// BOUNDARY
				output_cDataArray2D_uc1.getRef(x, y, 0) = 1;
				continue;
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
		// we are currently only able to process "unsigned char,3" data arrays.
		if (i_cPipelinePacket.type_info_name != typeid(CDataArray2D<unsigned char,3>).name())
		{
			std::cerr << "ERROR: Video Output is only able to process (char,3) arrays" << std::endl;
			exit(-1);
		}

		// unpack data
		CDataArray2D<unsigned char,3> *input = i_cPipelinePacket.getPayload<CDataArray2D<unsigned char,3> >();

		// copy data to input array
		input_cDataArray2D.resize(input->width, input->height);
		input_cDataArray2D.loadData(input->data);


		if (cParameters.stage_imageprocessing_output_flagfield)
		{
			apply_filter_flagfield();
		}
		else
		{
			// FILTER
			switch(cParameters.stage_imageprocessing_filter_id)
			{
			case 0:
				apply_filter_blur();
				break;

			case 1:
				apply_filter_threshold();
				break;

			default:	// no filter
				output_cDataArray2D_uc3.resize(input_cDataArray2D.width, input_cDataArray2D.height);
				output_cDataArray2D_uc3.loadData(input_cDataArray2D.data);
				break;
			}
		}

		// push modifications on image to pipeline
		pipeline_push();
	}
};

#endif /* CSTAGE_IMAGEINPUT_HPP_ */

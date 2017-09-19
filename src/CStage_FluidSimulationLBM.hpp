#ifndef CSTAGE_FLUIDSIMULATION_LBM_HPP_
#define CSTAGE_FLUIDSIMULATION_LBM_HPP_

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
class CStage_FluidSimulationLBM	:	public
	CPipelineStage
{
	// global parameters
	CParameters &cParameters;

	// input flag image
	CDataArray2D<unsigned char,1> input_cDataArray2D;

	//TODO input velocity field to set velocities behind bodies

	// processed output field (the first two components give the velocity the third one the density)
	CDataArray2D<float,3> output_Field;

	// previous f_i field
	CDataArray2D<float,9> fi_Old;
	
	// processed f_i field
	DataArray2D<float,9> fi_New;


public:
	/**
	 * constructor
	 */
	CStage_FluidSimulationLBM(CParameters &i_cParameters)	:
		CPipelineStage("FluidSimulationLBM"),
		cParameters(i_cParameters)

	// TODO initialize fi in the first iteration
	{
	}


public:
	/**
	 * manually triggered pushing of next image to the pipeline
	 */
	void pipeline_push()
	{
		

		
		// changed output to output_Field
		CPipelineStage::pipeline_push((CPipelinePacket&)output_Field);
	}



public:
	void simulation_timestep()
	{
		if (!input_cDataArray2D.isValidData())
			return;

		// initialization in the first iteration
		if (	output_Field.width != input_cDataArray2D.width || output_Field.height != input_cDataArray2D.height)
		{
			initialization();
		}





//TODO Calculate fi collision

//TODO Calculate fi streaming


//TODO Consider Boundary in Col & STream (1 = boundary, 2 = inflow, 3 = outflow)


//TODO Calculate macroscopic quantities


		pipeline_push();

	// Set fi_old = fi_new
	for (int y = 0; y < input_cDataArray2D.height; y++)
	{
		for (int x = 0; x < input_cDataArray2D.width; x++)
		{
			for (int i = 0; i < 9; i ++)
			{
					fi_Old.getRef(x,y,i) = fi_New.getRef(x,y,i);	
			}
		}
	}
	}

private:
	void initialization()
	{
		// initialize all the array sizes
		output_Field.resize(input_cDataArray2D.width, input_cDataArray2D.height);
		fi_New.resize(input_cDataArray2D.width, input_cDataArray2D.height);
		fi_Old.resize(input_cDataArray2D.width, input_cDataArray2D.height);
		// initialize fi in the first iteration
		for (int y = 0; y < input_cDataArray2D.height; y++)
		{
			for (int x = 0; x < input_cDataArray2D.width; x++)
			{
				for (int i = 0; i < 9; i ++)
				{
					fi_New.getRef(x,y,i) = -1;
					fi_Old.getRef(x,y,i) = -1;	
				}
			}
		}
	}


public:
	void main_loop_callback()
	{
		simulation_timestep();
	}

public: //TODO data input of boundary cells (and later also velocity of bodies)
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
		if (i_cPipelinePacket.type_info_name != typeid(CDataArray2D<unsigned char,1>).name())
		{
			std::cerr << "ERROR: Simulation input is only able to process (char,1) flag arrays" << std::endl;
			exit(-1);
		}

		// unpack data
		CDataArray2D<unsigned char,1> *input = i_cPipelinePacket.getPayload<CDataArray2D<unsigned char,1> >();

		// copy data to input array
		input_cDataArray2D.resize(input->width, input->height);
		input_cDataArray2D.loadData(input->data);
	}
};

#endif /* CSTAGE_IMAGEINPUT_HPP_ */

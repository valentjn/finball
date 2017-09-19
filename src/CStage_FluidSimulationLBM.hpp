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

	// input flag image (0,1 velocity, 2 type flag)
	CDataArray2D<float,3> input_cDataArray2D;

	// processed output field (the first two components give the velocity the third one the density)
	CDataArray2D<float,3> output_Field;

	// previous f_i field
	CDataArray2D<float,9> fi_Old;
	
	// processed f_i field
	CDataArray2D<float,9> fi_New;

	// internal f_i eqilibrium field
	CDataArray2d<float,9> fi_Eq;

public:
	// constructor
	CStage_FluidSimulationLBM(CParameters &i_cParameters)	:
		CPipelineStage("FluidSimulationLBM"),
		cParameters(i_cParameters)
	{
	}


public:
	// trigger pushing to the pipeline
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
	/*for (int y = 0; y < input_cDataArray2D.height; y++)
	{
		for (int x = 0; x < input_cDataArray2D.width; x++) //collision in cell (x,y)
		{
			float rho = 0;
			for (int i = 0; i < 9; i ++)
			{
				rho += fi_Old.getRef(x,y,i);
			}

			//TODO calculate cu , w(i)

			//TODO fi_Eq.getRef(x,y,i) = rho * w(i) * (1+cu + 0.5 * (cu)*(cu) + 3/2 *(ux*ux+uy*uy);

			for (int i =0; i<9; i++)
			{	//TODO define omega
				fi_New.getRef(x,y,i) = fi_Old.getRef(x,y,i) + omega * (fi_Eq.getRef(x,y,i) - fi_Old(x,yi));
			} 
		}
	}*/

//TODO Set fi_old = fi_new to use fi new during streaming
//TODO Calculate fi streaming


//TODO Consider Boundary in Col & STream (0 = fluid, 1 = boundary, 2 = inflow, 3 = outflow)


	// Calculate macroscopic quantities
	for (int y = 0; y < input_cDataArray2D.height; y++)
	{
		for (int x = 0; x < input_cDataArray2D.width; x++)
		{
			output_Field.getRef(x,y,2) = 0;
			for (int i = 0; i < 9; i ++)
			{
					output_Field.getRef(x,y,2) += fi_New.getRef(x,y,i);	//density
			}
			output_Field.getRef(x,y,0) = fi_New.getRef(x,y,1) - fi_New.getRef(x,y,3) + fi_New.getRef(x,y,5) - fi_New.getRef(x,y,6) - fi_New.getRef(x,y,7) + fi_New.getRef(x,y,8); //x Velocity
			output_Field.getRef(x,y,1) = fi_New.getRef(x,y,2) - fi_New.getRef(x,y,4)   + fi_New.getRef(x,y,5) + fi_New.getRef(x,y,6) - fi_New.getRef(x,y,7) - fi_New.getRef(x,y,8); //y Velocity
		}
	}

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
		fi_Eq.resize(input_cDataArray2D.width, input_cDataArray2D.height);
		// initialize fi in the first iteration
		for (int y = 0; y < input_cDataArray2D.height; y++)
		{
			for (int x = 0; x < input_cDataArray2D.width; x++)
			{
				for (int i = 0; i < 9; i ++)
				{
					//TODO set some equilibrium distribution for fixed rho here
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

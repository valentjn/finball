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
	/**
	 * global parameters
	 */
	CParameters &cParameters;

	/**
	 * input flag image
	 */
	CDataArray2D<unsigned char,1> input_cDataArray2D;

	/**
	 * processed velocity field
	 */
	CDataArray2D<float,2> output_cDataArray2D_f2;

	/**
	 * processed rgb field
	 */
	CDataArray2D<float,3> output_cDataArray2D_f3;

	/**
	 * simulation array to run simulation on
	 */
	CDataArray2D<float,2> simulationData_Buffer1;

public:
	/**
	 * constructor
	 */
	CStage_FluidSimulationLBM(CParameters &i_cParameters)	:
		CPipelineStage("FluidSimulationLBM"),
		cParameters(i_cParameters)
	{
	}


public:
	/**
	 * manually triggered pushing of next image to the pipeline
	 */
	void pipeline_push()
	{
		if (cParameters.stage_fluidsimulation_visualize_flagfield)
			CPipelineStage::pipeline_push((CPipelinePacket&)input_cDataArray2D);

		float scale = 10;
		for (int y = 0; y < simulationData_Buffer1.height; y++)
		{
			for (int x = 0; x < simulationData_Buffer1.width; x++)
			{
				output_cDataArray2D_f2.getRef(x,y,0) = simulationData_Buffer1.getRef(x,y,0)*scale + 0.5f;
				output_cDataArray2D_f2.getRef(x,y,1) = simulationData_Buffer1.getRef(x,y,1)*scale + 0.5f;
			}
		}

		CPipelineStage::pipeline_push((CPipelinePacket&)output_cDataArray2D_f2);
	}



public:
	void simulation_timestep()
	{
		if (!input_cDataArray2D.isValidData())
			return;

		if (	simulationData_Buffer1.width != input_cDataArray2D.width		||
				simulationData_Buffer1.height != input_cDataArray2D.height
		)
		{
			simulationData_Buffer1.resize(input_cDataArray2D.width, input_cDataArray2D.height);
			output_cDataArray2D_f2.resize(input_cDataArray2D.width, input_cDataArray2D.height);

			// reset
			for (int y = 0; y < output_cDataArray2D_f2.height; y++)
			{
				for (int x = 0; x < output_cDataArray2D_f2.width; x++)
				{
					simulationData_Buffer1.getRef(x, y, 0) = 0;
					simulationData_Buffer1.getRef(x, y, 1) = 0;
				}
			}
		}



		/*
		 * process input/output/boundary flags
		 */
		for (int y = 0; y < simulationData_Buffer1.height; y++)
		{
			for (int x = 0; x < simulationData_Buffer1.width; x++)
			{
				unsigned char flag = input_cDataArray2D.getRef(x,y);

				switch(flag)
				{
				case 1:	// boundary
					simulationData_Buffer1.getRef(x,y,0) = 0;
					simulationData_Buffer1.getRef(x,y,1) = 0;
					break;

				case 2:	// inflow
					simulationData_Buffer1.getRef(x,y,0) = 1;
					simulationData_Buffer1.getRef(x,y,1) = 0;
					break;

				case 3:	// outflow
					simulationData_Buffer1.getRef(x,y,0) = -1;
					simulationData_Buffer1.getRef(x,y,1) = 0;
					break;

				default:
					break;
				}
			}
		}


		/*
		 * averaging of velocity field
		 */
		for (int y = 0; y < simulationData_Buffer1.height; y++)
		{
			for (int x = 0; x < simulationData_Buffer1.width; x++)
			{
				unsigned char flag = input_cDataArray2D.getRef(x,y);

				static float s = 0.01;
				static float is = 1.0-s;

				float *v;
				float a,b;
				switch(flag)
				{
				case 0:	// fluid
					v = &simulationData_Buffer1.getClampedRef(x-1,y);
					a = v[0];
					b = v[1];

					v = &simulationData_Buffer1.getClampedRef(x+1,y);
					a += v[0];
					b += v[1];

					v = &simulationData_Buffer1.getClampedRef(x,y-1);
					a += v[0];
					b += v[1];

					v = &simulationData_Buffer1.getClampedRef(x,y+1);
					a += v[0];
					b += v[1];

					a *= 0.25;
					b *= 0.25;

					v = &simulationData_Buffer1.getRef(x,y);
					v[0] = v[0]*s+a*is;
					v[1] = v[1]*s+b*is;
					break;

				default:
					break;
				}
			}
		}

		pipeline_push();
	}


public:
	void main_loop_callback()
	{
		simulation_timestep();
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

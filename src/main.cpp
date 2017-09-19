//============================================================================
// Name        : fa_2014.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
//============================================================================

#include <iostream>

#include "visualization/CRenderer.hpp"
#include "CPipelineStage.hpp"
#include "CParameters.hpp"
#include "CStage_ImageInput.hpp"
#include "CStage_ImageProcessing.hpp"
#include "CStage_FluidSimulationLBM.hpp"
#include "CStage_RigidBodySimulation.hpp"
#include "CStage_VideoOutput.hpp"

#include "Levels/CLevel.hpp"

/*
 * program parameters
 */
CParameters cParameters;


void main_image_viewer()
{
	/*
	 * image input: read input file from a file
	 */
	CStage_ImageInput cStage_ImageInput(cParameters);

	/*
	 * video output: repeat output of an image
	 */
	//	CStage_VideoOutput cStage_VideoOutput(cParameters);
	CStage_VideoOutput cStage_VideoOutput(cParameters);

	/*
	 * here starts the connection between different pipeline stages
	 */

	/*
	 * connect video input directly to video output
	 */
	cStage_ImageInput.connectOutput(cStage_VideoOutput);

	// forward image data once
	cStage_ImageInput.pipeline_push();

	/*
	 * main processing queue
	 */
	while (!cParameters.exit)
	{
		// trigger image input to do something
		cStage_VideoOutput.main_loop_callback();
	}
}



void main_image_modifier()
{
	/*
	 * image input: read input file from a file
	 */
	CStage_ImageInput cStage_ImageInput(cParameters);

	/*
	 * video output: repeat output of an image
	 */
	CStage_VideoOutput cStage_VideoOutput(cParameters);

	/*
	 * here starts the connection between different pipeline stages
	 */

	/*
	 * connect video input directly to video output
	 */
	cStage_ImageInput.connectOutput(cStage_VideoOutput);

	/*
	 * also connect video output (mouse & keyboard) to image input
	 */
	cStage_VideoOutput.connectOutput(cStage_ImageInput);


	/*
	 * forward image data once to have something for output
	 */
	cStage_ImageInput.pipeline_push();

	/*
	 * main processing queue
	 */
	while (!cParameters.exit)
	{
		// trigger image input to do something
		cStage_VideoOutput.main_loop_callback();
	}
}



void main_image_modifier_and_filter()
{
	/*
	 * image input: read input file from a file
	 */
	CStage_ImageInput cStage_ImageInput(cParameters);

	/*
	 * image input: read input file from a file
	 */
	CStage_ImageProcessing cStage_ImageProcessing(cParameters);

	/*
	 * video output: repeat output of an image
	 */
	CStage_VideoOutput cStage_VideoOutput(cParameters);

	/*
	 * here starts the connection between different pipeline stages
	 */

	/*
	 * image input -> image processing
	 */
	cStage_ImageInput.connectOutput(cStage_ImageProcessing);

	/*
	 * image processing -> video output
	 */
	cStage_ImageProcessing.connectOutput(cStage_VideoOutput);

	/*
	 * video output -> image input modifications
	 */
	cStage_VideoOutput.connectOutput(cStage_ImageInput);


	// forward image data once to have something for output
	cStage_ImageInput.pipeline_push();

	/*
	 * main processing queue
	 */
	while (!cParameters.exit)
	{
		// trigger image input to do something
		cStage_VideoOutput.main_loop_callback();
	}
}


void main_sim_static_image()
{
	/*
	 * image input: read input file from a file
	 */
	CStage_ImageInput cStage_ImageInput(cParameters);

	/*
	 * image input: read input file from a file
	 */
	CStage_ImageProcessing cStage_ImageProcessing(cParameters);
	cParameters.stage_imageprocessing_output_flagfield = true;
	cParameters.stage_fluidsimulation_visualize_flagfield = true;

	/*
	 * fluid simulation test class
	 */
	CStage_FluidSimulationLBM cStage_FluidSimulationLBM(cParameters);

	/*
	 * video output: repeat output of an image
	 */
	CStage_VideoOutput cStage_VideoOutput(cParameters);

	/*
	 * here starts the connection between different pipeline stages
	 */
	cStage_ImageInput.connectOutput(cStage_ImageProcessing);
	cStage_ImageProcessing.connectOutput(cStage_FluidSimulationLBM);
	cStage_FluidSimulationLBM.connectOutput(cStage_VideoOutput);
	cStage_VideoOutput.connectOutput(cStage_ImageInput);


	/*
	 * forward image data once to have something for output
	 */
	cStage_ImageInput.pipeline_push();

	/*
	 * main processing queue
	 */
	while (!cParameters.exit)
	{
		// do simulation timestep
		cStage_FluidSimulationLBM.main_loop_callback();

		// trigger image input to do something
		cStage_VideoOutput.main_loop_callback();
	}
}

void main_game()
{
	CStage_RigidBodySimulation cStage_RBS(cParameters);
	CRenderer renderer;

	cStage_RBS.connectOutput(renderer);
	cStage_RBS.pipeline_push();
	
	CRenderObject object;

	object.position = glm::vec3{ 0.f, 0.f, 0.f };
	object.scale = 1.f;

	while (true) {
		// Poll available events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				return;
			}
		}

		// Render
		renderer.renderWorldObject(object);
		renderer.present();

		// Delay 15ms
		SDL_Delay(15);
	}
}


/**
 * Main C entry function
 */
int main(int argc, char *argv[])
{
	/*
	 * setup program parameters
	 */
	cParameters.setup(argc, argv);

	switch(cParameters.pipeline_id)
	{
		case 4:
			main_game();
			break;
		case 1:
			main_image_modifier();
			break;
		case 2:
			main_image_modifier_and_filter();
			break;
		case 3:
			main_sim_static_image();
			break;
		case 0:
			main_image_viewer();
			break;
	}
	return 0;
}

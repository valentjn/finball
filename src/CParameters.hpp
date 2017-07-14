#ifndef CPARAMETERS_HPP_
#define CPARAMETERS_HPP_


#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "CSDLInterface.hpp"

class CParameters
{
public:
	/**
	 * this parameter is tested during each pipeline step.
	 * in case that it's true, the program exists
	 */
	bool exit;

	/**
	 * verbosity level.
	 * the higher, the more output has to be generated
	 */
	int verbosity_level;

	/**
	 * image processing filter id
	 */
	int stage_imageprocessing_filter_id;

	/**
	 * image processing threshold value
	 */
	int stage_imageprocessing_threshold_value;

	/**
	 * image processing: output flagfield
	 */
	bool stage_imageprocessing_output_flagfield;


	/**
	 * visualize flag field for simulation
	 */
	bool stage_fluidsimulation_visualize_flagfield;

	/**
	 * for multithreading: how many threads are used for the pipelining
	 */
	int threading_number_of_threads_to_use;

	/**
	 * stage image input: default image to load
	 */
	std::string stage_image_input_path;

	/**
	 * pipeline id to process
	 */
	int pipeline_id;


	/**
	 * string to video device being used for, e.g., webcam input
	 */
	std::string video_device;

	/**
	 * request particular video width
	 */
	int input_video_width;
	/**
	 * request particular video height
	 */
	int input_video_height;

	/**
	 * return bool if processed
	 */
	bool key_down(char i_key)
	{
		switch(i_key)
		{
		case SDLK_j:
			stage_imageprocessing_filter_id--;
			std::cout << "Using filter id " << stage_imageprocessing_filter_id << std::endl;
			return true;

		case SDLK_k:
			stage_imageprocessing_filter_id++;
			std::cout << "Using filter id " << stage_imageprocessing_filter_id << std::endl;
			return true;


		case SDLK_g:
			stage_imageprocessing_threshold_value--;
			std::cout << "Using filter threshold value " << stage_imageprocessing_threshold_value << std::endl;
			return true;

		case SDLK_t:
			stage_imageprocessing_threshold_value++;
			std::cout << "Using filter threshold value " << stage_imageprocessing_threshold_value << std::endl;
			return true;

		case SDLK_v:
			stage_fluidsimulation_visualize_flagfield = !stage_fluidsimulation_visualize_flagfield;
			std::cout << "Visualize flag field: " << stage_fluidsimulation_visualize_flagfield << std::endl;
			return true;

		default:
			// unknown key
			break;
		}
		return false;
	}

	CParameters()
	{
		/*
		 * setup default parameter values
		 */
		exit = false;
		verbosity_level = -1;
		threading_number_of_threads_to_use = -1;
		stage_image_input_path = "data/fa_image_2012.jpg";

		stage_imageprocessing_filter_id = 0;
		stage_imageprocessing_threshold_value = 128;
		stage_imageprocessing_output_flagfield = false;
		stage_fluidsimulation_visualize_flagfield = false;

		pipeline_id = 0;

		video_device = "/dev/video0";
		input_video_width = -1;
		input_video_height = -1;
	}


	/**
	 * this function prints the program parameters to stdout
	 */
	void printMainParameterInfo(
			int i_argc,		/// number of arguments
			char *const i_argv[]	/// array of strings of arguments preprocessed by prefixed binary
	)
	{
		std::cout << "Parameters for " << i_argv[0] << std::endl;
		std::cout << "	-d [device]			Use video device (default: /dev/video0)" << std::endl;
		std::cout << "	-i [input image]	Input image (default: ../data/fa_image_2012.jpg)" << std::endl;
		std::cout << "	-v [level]			Verbosity level (default: -1)" << std::endl;
		std::cout << "	-p [pipeline id]	Pipeline id (default: 0)" << std::endl;
		std::cout << "	-w [video width]	(default: -1 [automatic])" << std::endl;
		std::cout << "	-h [video height]	(default: -1 [automatic])" << std::endl;
		::exit(EXIT_FAILURE);
	}



	/**
	 * setup the parameters given to the program.
	 *
	 * if any (optimistic!) of those parameters are invalid,
	 * noexit_printMainParameterInfo is called.
	 */
public:
	void setup(
		int i_argc,		/// number of arguments
		char *const i_argv[]	/// array of strings of arguments preprocessed by prefixed binary
	)
	{
		int optchar;
		const char *options = "v:d:i:n:p:w:h:";

		while ((optchar = getopt(i_argc, i_argv, options)) > 0)
		{
			switch(optchar)
			{
			/*
			 * verbosity level
			 */
			case 'v':
				verbosity_level = atoi(optarg);
				break;


			/*
			 * input video device
			 */
			case 'd':
				video_device = optarg;
				break;


			/*
			 * input image
			 */
			case 'i':
				stage_image_input_path = optarg;
				break;


			/*
			 * number of threads to use
			 */
			case 'n':
				threading_number_of_threads_to_use = atoi(optarg);
				break;


			/*
			 * pipeline id
			 */
			case 'p':
				pipeline_id = atoi(optarg);
				break;

			case 'w':
				input_video_width = atoi(optarg);
				break;
			case 'h':
				input_video_height = atoi(optarg);
				break;

			default:
				printMainParameterInfo(i_argc, i_argv);
				break;
			}
		}
	}

};

#endif /* CPARAMETERS_HPP_ */

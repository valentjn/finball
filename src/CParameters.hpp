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
		stage_image_input_path = "data/fa_image.jpg";

		stage_imageprocessing_filter_id = 0;
		stage_imageprocessing_threshold_value = 128;
		stage_imageprocessing_output_flagfield = false;
		stage_fluidsimulation_visualize_flagfield = false;

		pipeline_id = 0;
	}


	/**
	 * this function prints the program parameters to stdout
	 */
	void printMainParameterInfo(
			int i_argc,		/// number of arguments
			char *const i_argv[]	/// array of strings of arguments preprocessed by prefixed binary
	)
	{
		(void)i_argc;
		std::cout << "Parameters for " << i_argv[0] << std::endl;
		std::cout << "	-i [input image]	Input image (default: ../data/fa_image.jpg)" << std::endl;
		std::cout << "	-v [level]			Verbosity level (default: -1)" << std::endl;
		std::cout << "	-p [pipeline id]	Pipeline id (default: 0)" << std::endl;
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


			default:
				printMainParameterInfo(i_argc, i_argv);
				break;
			}
		}
	}

};

#endif /* CPARAMETERS_HPP_ */

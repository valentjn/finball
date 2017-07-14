#ifndef CPIPELINESTAGE_HPP_
#define CPIPELINESTAGE_HPP_

#include <iostream>
#include <stdexcept>
#include <string>

#include "CDataArray2D.hpp"
#include "CPipelinePacket.hpp"

/**
 * this class represents a pipeline stage.
 * each stage is triggered by previous stages.
 * if not, some appropriate actions to setup them
 * (creating threads, e.g.) have to be taken.
 */
class CPipelineStage
{
	CPipelineStage *input_cPipelineStage[4];
	CPipelineStage *output_cPipelineStage[4];

	const char *identifier;


public:
	CPipelineStage()
	{
		setup("");
	}

public:
	virtual ~CPipelineStage()
	{
	}

public:
	CPipelineStage(const char *i_identifier)
	{
		setup(i_identifier);
	}


private:
	/**
	 * setup for constructors with identifier string
	 */
	void setup(
			const char *i_identifier	// identifier string
	)
	{
		identifier = i_identifier;

		input_cPipelineStage[0] = nullptr;
		input_cPipelineStage[1] = nullptr;
		input_cPipelineStage[2] = nullptr;
		input_cPipelineStage[3] = nullptr;

		output_cPipelineStage[0] = nullptr;
		output_cPipelineStage[1] = nullptr;
		output_cPipelineStage[2] = nullptr;
		output_cPipelineStage[3] = nullptr;
	}

	void printPipeline(int depth = 0)
	{
		for (int i = 0; i < depth*2; i++)
			std::cout << " ";

		std::cout << "(" << depth << ") " << identifier << std::endl;

		for (int i = 0; i < 4; i++)
		{
			if (output_cPipelineStage[i] != nullptr)
				output_cPipelineStage[i]->printPipeline(depth+1);
		}
	}

private:
	void connectInput(
			CPipelineStage &io_cPipelineStage
	)
	{
		// search for next free input
		for (int i = 0; i < 4; i++)
		{
			if (input_cPipelineStage[i] == nullptr)
			{
				input_cPipelineStage[i] = &io_cPipelineStage;
				return;
			}
		}

		std::cerr << "No more inputs available for " << identifier << std::endl;
		exit(-1);
	}

public:
	void connectOutput(
			int i_output_id,
			CPipelineStage &io_cPipelineStage
	)
	{
		output_cPipelineStage[i_output_id] = &io_cPipelineStage;
		output_cPipelineStage[i_output_id]->connectInput(*this);
	}



public:
	void connectOutput(
			CPipelineStage &io_cPipelineStage
	)
	{
		for (int i = 0; i < 4; i++)
		{
			if (output_cPipelineStage[i] == nullptr)
			{
				output_cPipelineStage[i] = &io_cPipelineStage;
				output_cPipelineStage[i]->connectInput(*this);
				return;
			}
		}

		throw(std::runtime_error(std::string("No stages left for ")+identifier));
	}


public:
	void pipeline_push(
			CPipelinePacket &i_cPipelinePacket
	)
	{
		for (int i = 0; i < 4; i++)
		{
			if (output_cPipelineStage[i] != 0)
				output_cPipelineStage[i]->pipeline_pull(i_cPipelinePacket);
		}
	}

public:
	virtual void pipeline_process_input(CPipelinePacket &i_cPipelinePacket) = 0;


private:
	/**
	 * this method is executed by the pipeline to pull data from an input state
	 */
	void pipeline_pull(
			CPipelinePacket &i_cPipelinePacket
	)
	{
		pipeline_process_input(i_cPipelinePacket);
	}

};

#endif /* CPIPELINESTAGE_HPP_ */

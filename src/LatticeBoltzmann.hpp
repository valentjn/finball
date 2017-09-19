#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include <memory>

#include "Parameters.hpp"
#include "Level.hpp"
#include "LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "FICell.hpp"
#include "glm/vec3.hpp"

using namespace glm;

class LatticeBoltzmann {
private:
	Parameters &parameters;
	Level &level;

	// previous f_i field
	Array2D<FICell> fi_Old;

	// processed f_i field
	Array2D<FICell> fi_New;

public:
	LatticeBoltzmann(Parameters &parameters, Level &level) :
		parameters(parameters),
		level(level),
		fi_Old(level.width, level.height),
		fi_New(level.width, level.height)
	{
		// initialize fi in the first iteration
		for (int y = 0; y < level.height; y++)
		{
			for (int x = 0; x < level.width; x++)
			{
				fi_New.setValue(x, y, FICell());
				fi_Old.setValue(x, y, FICell());
			}
		}
	}

	void compute(const LatticeBoltzmannInput &input,
				 LatticeBoltzmannOutput &output)
	{
		if (output.matrix.get() == nullptr)
		{
			output.matrix = std::make_unique<Array2D<vec3>>(level.width, level.height);
		}

		//TODO Calculate fi collision
		/*for (int y = 0; y < input_cDataArray2D.height; y++)
		{
			for (int x = 0; x < input_cDataArray2D.width; x++)
			{
				if (input_cDataArray2D.getRef(x,y)
					for (int i = 0; i < 9; i ++)
					{

					}
			}
		}*/

		//TODO Calculate fi streaming

		//TODO Consider Boundary in Col & STream (0 = fluid, 1 = boundary, 2 = inflow, 3 = outflow)

		// Calculate macroscopic quantities
		for (int y = 0; y < level.height; y++)
		{
			for (int x = 0; x < level.width; x++)
			{
				output.matrix->getRef(x,y).z = 0;
				for (int i = 0; i < 9; i ++)
				{
						output.matrix->getRef(x,y).z += fi_New.getRef(x,y)[i];	//density
				}
				output.matrix->getRef(x,y).x = fi_New.getRef(x,y)[1] - fi_New.getRef(x,y)[3] + fi_New.getRef(x,y)[5] - fi_New.getRef(x,y)[6] - fi_New.getRef(x,y)[7] + fi_New.getRef(x,y)[8]; //x Velocity
				output.matrix->getRef(x,y).y = fi_New.getRef(x,y)[2] - fi_New.getRef(x,y)[4] + fi_New.getRef(x,y)[5] + fi_New.getRef(x,y)[6] - fi_New.getRef(x,y)[7] - fi_New.getRef(x,y)[8]; //y Velocity
			}
		}

		// Set fi_old = fi_new
		fi_Old.loadData(fi_New.getRawData());
	}
};

#endif

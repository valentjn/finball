#ifndef LATTICE_BOLTZMANN_INPUT_HPP_
#define LATTICE_BOLTZMANN_INPUT_HPP_

#include <memory>

#include "UserInputOutput.hpp"
#include "RigidBodyOutput.hpp"
#include "Array2D.hpp"
#include "glm/vec3.hpp"

class LatticeBoltzmannInput {
public:
	// input flag image (0,1 velocity, 2 type flag)
	std::unique_ptr<Array2D<glm::vec3>> matrix = nullptr;

	LatticeBoltzmannInput(){}

	LatticeBoltzmannInput(const UserInputOutput &userInputOutput,
						  const RigidBodyOutput &rigidBodyOutput)
	{
		//TODO set matrix to some value
	}
};

#endif

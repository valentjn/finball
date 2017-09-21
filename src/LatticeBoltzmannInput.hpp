#ifndef LATTICE_BOLTZMANN_INPUT_HPP_
#define LATTICE_BOLTZMANN_INPUT_HPP_

#include <memory>

#include "Array2D.hpp"
#include "RigidBodyPhysicsOutput.hpp"
#include "UserInputOutput.hpp"
#include "glm/vec3.hpp"

class LatticeBoltzmannInput {
public:
    // input flag image (0,1 velocity, 2 type flag)
    std::unique_ptr<Array2D<glm::vec3>> matrix = nullptr;

    LatticeBoltzmannInput() {}

    LatticeBoltzmannInput(const UserInputOutput &userInputOutput,
                          const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput) :
	matrix(std::make_unique<Array2D<glm::vec3>>(3,3)) {
        // TODO set matrix to some value
	Array2D<glm::vec3>* matrixPointer = matrix.get();
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			matrixPointer->value(i,j) = glm::vec3(0,0,0);
		}
	}
    }
};

#endif

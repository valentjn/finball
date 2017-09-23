#ifndef RIGID_BODY_PHYSICS_INPUT_HPP_
#define RIGID_BODY_PHYSICS_INPUT_HPP_

#include "../LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "../../ext/glm/vec2.hpp"

class RigidBodyPhysicsInput {
private:
	const Array2D<FICell>* afterStream;
	const Array2D<FICell>* preStream;
	const int * cx;
	const int * cy;
public:
    RigidBodyPhysicsInput() : afterStream(nullptr), preStream(nullptr),
	cx(nullptr), cy(nullptr) {}

    RigidBodyPhysicsInput(const UserInputOutput &userInputOutput,
                          const LatticeBoltzmannOutput &latticeBoltzmannOutput) :
						  afterStream(&(latticeBoltzmannOutput.afterstream)),
						  preStream(&(latticeBoltzmannOutput.prestream)),
						  cx(latticeBoltzmannOutput.cx),
						  cy(latticeBoltzmannOutput.cy) {}

	void computeImpulseAt(const glm::vec2& pos, glm::vec2& impulse) {
		impulse.x = pos.x/2.0;
		impulse.y = pos.y/2.0;
	}

	void computeImpulseAt(const int posX, const int posY, glm::vec2& impulse) {
		impulse = glm::vec2(0.0,0.0);
		for (int i = 0; i < 9; ++i)
		{
			impulse.x += cx[i]*(afterStream->value(posX,posY)[i]+preStream->value(posX,posY)[i]);
			impulse.y += cy[i]*(afterStream->value(posX,posY)[i]+preStream->value(posX,posY)[i]);
		}
	}

	~RigidBodyPhysicsInput() {
		afterStream = nullptr;
		preStream = nullptr;
	}

};

#endif

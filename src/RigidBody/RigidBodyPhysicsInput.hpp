#ifndef RIGID_BODY_PHYSICS_INPUT_HPP_
#define RIGID_BODY_PHYSICS_INPUT_HPP_

#include "../LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "../../ext/glm/vec2.hpp"

class RigidBodyPhysicsInput {
private:
	const Array2D<FICell>* beforeStream;
	const Array2D<FICell>* preStream;
public:
    RigidBodyPhysicsInput() : beforeStream(nullptr), preStream(nullptr) {}

    RigidBodyPhysicsInput(const UserInputOutput &userInputOutput,
                          const LatticeBoltzmannOutput &latticeBoltzmannOutput) :
						  beforeStream(&(latticeBoltzmannOutput.prestream)),
						  preStream(&(latticeBoltzmannOutput.afterstream)) {}

	void computeImpulseAt(const glm::vec2& pos, glm::vec2& impulse) {
		impulse.x = pos.x/2.0;
		impulse.y = pos.y/2.0;
	}

	void computeImpulseAt(const int& posX, const int& posY, glm::vec2& impulse) {
		impulse.x = posX/2.0;
		impulse.y = posY/2.0;
	}

	~RigidBodyPhysicsInput() {
		beforeStream = nullptr;
		preStream = nullptr;
	}

};

#endif

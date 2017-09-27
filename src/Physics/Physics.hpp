#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include <LatticeBoltzmann/LatticeBoltzmann.hpp>
#include <LatticeBoltzmann/LatticeBoltzmannInput.hpp>
#include <LatticeBoltzmann/LatticeBoltzmannOutput.hpp>
#include <RigidBody/RigidBodyPhysics.hpp>
#include <RigidBody/RigidBodyPhysicsInput.hpp>
#include <RigidBody/RigidBodyPhysicsOutput.hpp>
#include <LevelDesign/Level.hpp>

struct PhysicsInput
{
	UserInputOutput userInputOutput;

	void process(const UserInputOutput& uio)
	{
		userInputOutput = uio;
	}
};

struct PhysicsOutput
{
	LatticeBoltzmannOutput latticeBoltzmannOutput;
	RigidBodyPhysicsOutput rigidBodyPhysicsOutput;
};

class Physics
{
	const Level& level;

	LatticeBoltzmann latticeBoltzmann;
	RigidBodyPhysics rigidBodyPhysics;

	LatticeBoltzmannOutput latticeBoltzmannOutput;

	LatticeBoltzmannInput latticeBoltzmannInput;
	RigidBodyPhysicsInput rigidBodyPhysicsInput;

	LatticeBoltzmannOutput* prevLbmOutput;

public:
	Physics(const Level& level)
		: level(level)
		, latticeBoltzmann(level)
		, rigidBodyPhysics(level)
	{
		latticeBoltzmannOutput.velocity = Array2D<glm::vec2>(level.width, level.height);
		latticeBoltzmannOutput.density = Array2D<float>(level.width, level.height);
		latticeBoltzmannOutput.prestream = Array2D<FICell>(level.width, level.height);
		latticeBoltzmannOutput.afterstream = Array2D<FICell>(level.width, level.height);
	}

	void compute(const PhysicsInput& input, PhysicsOutput& output)
	{
		if (output.rigidBodyPhysicsOutput.grid_objects.width() == 0) {
			output.rigidBodyPhysicsOutput.grid_objects = Array2D<Level::CellType>(level.width, level.height);
			output.rigidBodyPhysicsOutput.grid_velocities = Array2D<glm::vec2>(level.width, level.height);
		}

		rigidBodyPhysicsInput.process(input.userInputOutput, latticeBoltzmannOutput);
		rigidBodyPhysics.compute(rigidBodyPhysicsInput, output.rigidBodyPhysicsOutput);

		latticeBoltzmannInput.process(output.rigidBodyPhysicsOutput);
		latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);

		output.latticeBoltzmannOutput = latticeBoltzmannOutput;
	}
};


#endif
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

public:
	Physics(const Level& level)
		: level(level)
		, latticeBoltzmann(level)
		, rigidBodyPhysics(level)
	{}

	void initOutput(PhysicsOutput& output)
	{
		rigidBodyPhysics.initOutput(output.rigidBodyPhysicsOutput);
		latticeBoltzmann.initOutput(latticeBoltzmannOutput);
	}

	void compute(const PhysicsInput& input, PhysicsOutput& output)
	{
		rigidBodyPhysicsInput.process(input.userInputOutput, latticeBoltzmannOutput);
		rigidBodyPhysics.compute(rigidBodyPhysicsInput, output.rigidBodyPhysicsOutput);

		latticeBoltzmannInput.process(output.rigidBodyPhysicsOutput);
		latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);

		output.latticeBoltzmannOutput = latticeBoltzmannOutput;
	}
};


#endif
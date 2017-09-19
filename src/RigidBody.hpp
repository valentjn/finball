#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include "Parameters.hpp"
#include "Level.hpp"
#include "RigidBodyInput.hpp"
#include "RigidBodyOutput.hpp"

class RigidBody {
private:
    Parameters &parameters;
    Level &level;

public:
    RigidBody(Parameters &parameters, Level &level) :
        parameters(parameters),
        level(level)
    {}

    void compute(const RigidBodyInput &input,
                 RigidBodyOutput &rigidBodyOutput)
    {
        // TODO
    }
};

#endif

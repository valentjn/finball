#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include "Parameters.hpp"
#include "RigidBodyInput.hpp"
#include "RigidBodyOutput.hpp"

class RigidBody {
private:
    Parameters &parameters;

public:
    RigidBody(Parameters &parameters) : parameters(parameters)
    {}

    void compute(const RigidBodyInput *input,
                 RigidBodyOutput *rigidBodyOutput)
    {
        // TODO
    }
};

#endif

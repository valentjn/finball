#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include "Parameters.hpp"
#include "UserInputData.hpp"
#include "LatticeBoltzmannData.hpp"
#include "RigidBodyData.hpp"

class RigidBody {
private:
    Parameters &parameters;

public:
    RigidBody(Parameters &parameters) : parameters(parameters)
    {}

    void compute(const UserInputData *userInputData,
                 const LatticeBoltzmannData *latticeBoltzmannData,
                 RigidBodyData *rigidBodyData)
    {
        // TODO
    }
};

#endif

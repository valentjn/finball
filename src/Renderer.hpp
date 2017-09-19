#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "Parameters.hpp"
#include "LatticeBoltzmannData.hpp"
#include "RigidBodyData.hpp"

class Renderer {
private:
    Parameters &parameters;

public:
    Renderer(Parameters &parameters) : parameters(parameters)
    {}

    void render(const LatticeBoltzmannData *latticeBoltzmannData,
                RigidBodyData *rigidBodyData)
    {
        // TODO
    }
};

#endif

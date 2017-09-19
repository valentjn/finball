#ifndef RENDERER_INPUT_HPP_
#define RENDERER_INPUT_HPP_

#include "RigidBodyOutput.hpp"
#include "LatticeBoltzmannOutput.hpp"

class RendererInput {
public:
    RendererInput() {}

    RendererInput(const RigidBodyOutput *rigidBodyOutput,
                  const LatticeBoltzmannOutput *latticeBoltzmannOutput)
    {}
};

#endif

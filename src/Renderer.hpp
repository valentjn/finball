#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "Parameters.hpp"
#include "RendererInput.hpp"

class Renderer {
private:
    Parameters &parameters;

public:
    Renderer(Parameters &parameters) : parameters(parameters)
    {}

    void render(const RendererInput &rendererInput)
    {
        // TODO
    }
};

#endif

#ifndef COMPGAMELOGIC_HPP_
#define COMPGAMELOGIC_HPP_

#include <renderer/RenderObject.hpp>

#include <SDL2/SDL.h>

class CompGameLogic
{
    RenderObject m_object;

public:
    // output type of this component
    struct OutputData
    {
        std::vector<RenderObject> objects_to_render;
    };

    CompGameLogic()
    {
        m_object.position = { 0.f, 0.f, 0.f };
        m_object.scale = 1.f;
    }

	// writes the output of this component to output
    bool update(OutputData& output)
    {
        output.objects_to_render.push_back(m_object);
        return true;
    }
};

#endif

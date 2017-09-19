#ifndef INTERGAMELOGICTORENDERER_HPP_
#define INTERGAMELOGICTORENDERER_HPP_

#include <gamelogic/CompGameLogic.hpp>
#include <renderer/CompRenderer.hpp>

class InterGameLogicToRenderer {
public:
    using from_comp_t = CompGameLogic;
    using to_comp_t = CompRenderer;

    static void process(const typename CompGameLogic::OutputData& data_from, CompRenderer& comp_to)
    {
        for (const auto& object : data_from.objects_to_render) {
            comp_to.renderWorldObject(object);
        }
    }
};

#endif

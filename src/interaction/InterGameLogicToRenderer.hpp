#ifndef INTERGAMELOGICTORENDERER_HPP_
#define INTERGAMELOGICTORENDERER_HPP_

#include <gamelogic/CompGameLogic.hpp>
#include <renderer/CompRenderer.hpp>

class InterGameLogicToRenderer {
    using from_comp_t = CompGameLogic;
    using to_comp_t = CompRenderer;

    static void process(const typename CompGameLogic::OutputData& data_from, CompRenderer& comp_to);
};

#endif

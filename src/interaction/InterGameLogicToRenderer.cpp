#include <gamelogic/InterGameLogicToRenderer.hpp>

void InterGameLogicToRenderer::process(const typename from_comp_t::OutputData& data_from, typename to_comp_t& comp_to)
{
    RenderObject obj;
    obj.position = { 0.f, 0.f, 0.f };
    obj.scale = 1.f;
    comp_to.renderWorldObject(obj);
}

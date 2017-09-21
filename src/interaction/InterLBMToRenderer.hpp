#ifndef INTERLBMTORENDERER_HPP_
#define INTERLBMTORENDERER_HPP_

#include <lbm/LaticeBoltzmann.hpp>
#include <renderer/CompRenderer.hpp>

class InterLBMToRenderer {
public:
    using from_comp_t = CompLatticeBoltzmann;
    using to_comp_t = CompRenderer;

    static void process(const typename CompLatticeBoltzmann::OutputData& data_from, CompRenderer& comp_to)
    {
	    comp_to.setFluidVecs(*data_from.matrix);
    }
};


#endif

#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include <memory>

#include <glm/vec3.hpp>

#include "Array2D.hpp"
#include "LatticeBoltzmann/FICell.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "Level.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "LatticeBoltzmannInput.hpp"

using namespace glm;

class LatticeBoltzmann {
private:
    Level &level;

    // previous f_i field
    Array2D<FICell> fi_Old;

    // processed f_i field
    Array2D<FICell> fi_New;

    // quadrature weights for approximating equilibrium distribution
    float w[9] = {4. / 9.,  1. / 9.,  1. / 9.,  1. / 9., 1. / 9.,
                  1. / 36., 1. / 36., 1. / 36., 1. / 36.};
    int cx[9] = {0, 1, 0, -1, 0, 1, -1, -1, 1};
    int cy[9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
    int opp[9] = {0, 3, 4, 1, 2, 7, 8, 5, 6};

    bool isBoundaryValid(const Array2D<Level::CellType> &flagfield);

	void HandleCollisions(const LatticeBoltzmannInput &input);
	void ReinitilizeFI(LatticeBoltzmannOutput &output);
	void Output(LatticeBoltzmannOutput &output);
	void Stream(const LatticeBoltzmannInput &input);
	void HandleBoundaries(const LatticeBoltzmannInput &input);
	void outputFiPrestream(LatticeBoltzmannOutput &output);
	void initFiObstacles(const LatticeBoltzmannInput &input);


public:
    LatticeBoltzmann(Level &level)
        : level(level), fi_Old(level.width, level.height), fi_New(level.width, level.height) {
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                // set initial values
                for (int z = 0; z < 9; z++) {
                    fi_New.value(x, y)[z] = w[z] * 0.1 / (0.1 + x);
                    fi_Old.value(x, y)[z] = w[z] * 0.1 / (0.1 + x);
                }
            }
        }
    }

    void compute(const LatticeBoltzmannInput &input, LatticeBoltzmannOutput &output);
};

#endif

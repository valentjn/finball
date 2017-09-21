#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include <memory>

#include "FICell.hpp"
#include "LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "Level.hpp"
#include "Parameters.hpp"
#include "glm/vec3.hpp"

using namespace glm;

class LatticeBoltzmann {
private:
    Parameters &parameters;
    Level &level;

    // previous f_i field
    Array2D<FICell> fi_Old;

    // processed f_i field
    Array2D<FICell> fi_New;

    // internal f_i equilibrium field
    Array2D<FICell> fi_Eq;

    // quadrature weights for approximating equilibrium distribution
    int w [9] = {4/9, 1/9, 1/9, 1/9, 1/9, 1/36, 1/36, 1/36, 1/36};
    int cx [9] = {0, 1, 0, -1, 0 , 1, -1, -1, 1};
    int cy [9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
    int opp [9] = {0, 3, 4, 1, 2, 7, 8, 5, 6}; 

public:
    LatticeBoltzmann(Parameters &parameters, Level &level)
        : parameters(parameters), level(level), fi_Old(level.width, level.height),
          fi_New(level.width, level.height), fi_Eq(level.width, level.height) {
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                // TODO set some equilibrium distribution for fixed rho here
                fi_New.setValue(x, y, FICell());
                fi_Old.setValue(x, y, FICell());
            }
        }
    }

    void compute(const LatticeBoltzmannInput &input, LatticeBoltzmannOutput &output) {
        if (output.matrix.get() == nullptr) {
            output.matrix = std::make_unique<Array2D<vec3>>(level.width, level.height);
        }

        // TODO Calculate fi collision
        /*for (int y = 0; y < level.height; y++)
        {
                for (int x = 0; x < level.width; x++)
                {	//TODO check that the cell is not an obstacle
                        float rho = 0;
                        for (int i = 0; i < 9; i ++)
                        {
                                rho += fi_Old.getRef(x,y)[i];
                        }

                        //TODO calculate cu

                        fi_Eq.getRef(x,y)[i] = rho * w(i) * (1+cu + 0.5 *
        (cu)*(cu) + 3/2 *(ux*ux+uy*uy);

                        for (int i = 0; i < 9; i++)
                        {	//TODO define omega
                                fi_New.getRef(x,y)[i] = fi_Old.getRef(x,y)[i] +
        omega * (fi_Eq.getRef(x,y)[i] - fi_Old.getRef(x,y)[i]);
                        }
                }
        }*/

        // TODO Set fi_old = fi_new to use fi new during streaming
        // TODO Calculate fi streaming

        // TODO Consider Boundary in Col & STream (0 = fluid, 1 = boundary, 2 =
        // inflow, 3 = outflow)

        // Calculate macroscopic quantities for the output
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                output.matrix->getRef(x, y).z = 0;
                for (int i = 0; i < 9; i++) {
                    output.matrix->getRef(x, y).z += fi_New.getRef(x, y)[i]; // density
                }
                output.matrix->getRef(x, y).x = fi_New.getRef(x, y)[1] - fi_New.getRef(x, y)[3] +
                                                fi_New.getRef(x, y)[5] - fi_New.getRef(x, y)[6] -
                                                fi_New.getRef(x, y)[7] +
                                                fi_New.getRef(x, y)[8]; // x Velocity
                output.matrix->getRef(x, y).y = fi_New.getRef(x, y)[2] - fi_New.getRef(x, y)[4] +
                                                fi_New.getRef(x, y)[5] + fi_New.getRef(x, y)[6] -
                                                fi_New.getRef(x, y)[7] -
                                                fi_New.getRef(x, y)[8]; // y Velocity
            }
        }

        // Set fi_old = fi_new
        fi_Old.loadData(fi_New.getRawData());
    }
};

#endif

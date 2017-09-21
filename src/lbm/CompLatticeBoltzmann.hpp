#ifndef COMPLATTICEBOLTZMANN_HPP_
#define COMPLATTICEBOLTZMANN_HPP_

class CompLatticeBoltzmann
{
    // previous f_i field
    Array2D<FICell> fi_Old;

    // processed f_i field
    Array2D<FICell> fi_New;

    // internal f_i equilibrium field
    Array2D<FICell> fi_Eq;

public:
	// output type of this component
    struct OutputData
	{
        // processed output field (the first two components give the velocity the
        // third one the density)
        std::unique_ptr<Array2D<glm::vec3>> matrix;
	};

    CompLatticeBoltzmann()
        : fi_Old(level.width, level.height),
          fi_New(level.width, level.height),
          fi_Eq(level.width, level.height) {
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                // TODO set some equilibrium distribution for fixed rho here
                fi_New.value(x, y) = FICell();
                fi_Old.value(x, y) = FICell();
            }
        }
    }

	// writes the output of this component to output
    bool update(OutputData& output)
	{   
        if (output.matrix.get() == nullptr) {
            output.matrix = std::make_unique<Array2D<vec3>>(level.width, level.height);
        }

        // TODO Calculate fi collision
        /*for (int y = 0; y < level.height; y++)
        {
                for (int x = 0; x < level.width; x++)
                {
                        float rho = 0;
                        for (int i = 0; i < 9; i ++)
                        {
                                rho += fi_Old.getRef(x,y)[i];
                        }

                        //TODO calculate cu , w(i)

                        //TODO fi_Eq.getRef(x,y)[i] = rho * w(i) * (1+cu + 0.5 *
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

        // Calculate macroscopic quantities
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                output.matrix->value(x, y).z = 0;
                for (int i = 0; i < 9; i++) {
                    output.matrix->value(x, y).z += fi_New.value(x, y)[i]; // density
                }
                output.matrix->value(x, y).x = fi_New.value(x, y)[1] - fi_New.value(x, y)[3] +
                                                fi_New.value(x, y)[5] - fi_New.value(x, y)[6] -
                                                fi_New.value(x, y)[7] +
                                                fi_New.value(x, y)[8]; // x Velocity
                output.matrix->value(x, y).y = fi_New.value(x, y)[2] - fi_New.value(x, y)[4] +
                                                fi_New.value(x, y)[5] + fi_New.value(x, y)[6] -
                                                fi_New.value(x, y)[7] -
                                                fi_New.value(x, y)[8]; // y Velocity
            }
        }

        // Set fi_old = fi_new
	    fi_Old = fi_New;
	}
};

#endif

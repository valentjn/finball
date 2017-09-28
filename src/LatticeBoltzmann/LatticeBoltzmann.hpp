#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include <memory>

#include <glm/vec3.hpp>
#include <chrono>

#include "Array2D.hpp"
#include "LatticeBoltzmann/FICell.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "LevelDesign/Level.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "LatticeBoltzmannInput.hpp"

using namespace glm;

class LatticeBoltzmann
{
public:
	static const int cx[9];
	static const int cy[9];

private:
	std::chrono::duration<float> measuredTimes[5];

    const Level &level;

    // previous f_i field
    Array2D<FICell> fi_Old;

    // processed f_i field
    Array2D<FICell> fi_New;

    // quadrature weights for approximating equilibrium distribution
    static const float w[9];
    static const int opp[9];
    static const int iter;

    bool isBoundaryValid(const Array2D<Level::CellType> &flagfield);
    bool isSane(float fiValue, int x, int y, int i);


	void handleCollisions(const LatticeBoltzmannInput &input);
	void reinitializeFI(LatticeBoltzmannOutput &output);
	void Output(LatticeBoltzmannOutput &output);
	void stream(const LatticeBoltzmannInput &input);
	void handleBoundaries(const LatticeBoltzmannInput &input);
	void initFiObstacles(const LatticeBoltzmannInput &input);
	inline float handleWindShadow(const LatticeBoltzmannInput &input, int x, int y);

public:
    LatticeBoltzmann(const Level &level)
        : level(level), fi_Old(level.width, level.height), fi_New(level.width, level.height) {
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                // set initial values
                for (int z = 0; z < 9; z++) {
                    fi_New.value(x, y)[z] = w[z] * 0.5;
                    fi_Old.value(x, y)[z] = w[z] * 0.5;
                }
            }
        }
			for (int i = 0; i < 5; i++) {
				measuredTimes[i] = std::chrono::duration<float>(0.);
			}
    }

	void initOutput(LatticeBoltzmannOutput& output);

    void compute(const LatticeBoltzmannInput &input, LatticeBoltzmannOutput &output);

	void step(const LatticeBoltzmannInput &input, LatticeBoltzmannOutput &output);
};

#endif

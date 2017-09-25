#include <memory>
#include <iostream>

#include <glm/vec3.hpp>

#include "FICell.hpp"
#include "LatticeBoltzmann.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "LatticeBoltzmannInput.hpp"
#include "Level.hpp"
#include "Array2D.hpp"
#include <omp.h>

using namespace glm;

void LatticeBoltzmann::compute(const LatticeBoltzmannInput &input, LatticeBoltzmannOutput &output)
{
	// Check flag field
	assert(isBoundaryValid(input.flagfield));
	
	for (int i = 0; i < 5; i++) {
		step(input, output);
	}

	Output(output);
}

void LatticeBoltzmann::step(const LatticeBoltzmannInput &input, LatticeBoltzmannOutput &output)
{
	handleCollisions(input);

	// set f_i in obstacles to 0
	initFiObstacles(input);


	stream(input);
	handleBoundaries(input);
	
	reinitilizeFI(output);

}

void LatticeBoltzmann::initFiObstacles(const LatticeBoltzmannInput &input)
{
	for (int y = 0; y < level.height; ++y) {
		for (int x = 0; x < level.width; ++x) {
			if (input.flagfield.value(x, y) == Level::OBSTACLE) {
				for (int i = 0; i < 9; ++i) {
					fi_Old.value(x, y)[i] = 0.0;
					fi_New.value(x, y)[i] = 0.0;
				}
			}
		}
	}
}

void LatticeBoltzmann::handleBoundaries(const LatticeBoltzmannInput &input)
{

	for (int y = 0; y < level.height; ++y) {
		for (int x = 0; x < level.width; ++x) {
			switch (input.flagfield.value(x, y)) {
				case Level::OBSTACLE:
					for (int z = 1; z < 9; ++z) {
						if (fi_New.value(x, y)[z] != 0.0) {
						fi_New.value(x + cx[opp[z]], y + cy[opp[z]])[opp[z]] =
								fi_New.value(x, y)[z];
						fi_New.value(x, y)[z] = 0.0;
						}
					}
					break;
				case Level::INFLOW:
					for (int z = 0; z < 9; z++) {
						fi_New.value(x, y)[z] = w[z] * 1;
						fi_Old.value(x, y)[z] = w[z] * 1;

						bool condition = 0 <= (x + cx[z]) && (x + cx[z]) < level.width && 0 <= (y + cy[z]) &&
										 (y + cy[z]) < level.height &&
										 input.flagfield.value(x + cx[z], y + cy[z]) ==
										 Level::FLUID;
						if (condition) {
							fi_New.value(x + cx[z], y + cy[z])[z] = fi_New.value(x, y)[z];
						}
					}
					break;
				case Level::OUTFLOW:
					for (int z = 0; z < 9; z++) {
						fi_New.value(x, y)[z] = 0;
						fi_Old.value(x, y)[z] = 0;
					}
					break;
				default:
					break;
			}
		}
	}
}

void LatticeBoltzmann::stream(const LatticeBoltzmannInput &input)
{
	for (int y = 1; y < this->level.height - 1; ++y) {
		for (int x = 1; x < this->level.width - 1; ++x) {
			// check for boundary
			if (input.flagfield.value(x, y) == Level::FLUID) {
				this->fi_New.value(x, y)[0] = this->fi_Old.value(x, y)[0];
				this->fi_New.value(x + 1, y)[1] = this->fi_Old.value(x, y)[1];
				this->fi_New.value(x + 1, y + 1)[5] = this->fi_Old.value(x, y)[5];
				this->fi_New.value(x, y + 1)[2] = this->fi_Old.value(x, y)[2];
				this->fi_New.value(x - 1, y + 1)[6] = this->fi_Old.value(x, y)[6];
				this->fi_New.value(x - 1, y)[3] = this->fi_Old.value(x, y)[3];
				this->fi_New.value(x - 1, y - 1)[7] = this->fi_Old.value(x, y)[7];
				this->fi_New.value(x, y - 1)[4] = this->fi_Old.value(x, y)[4];
				this->fi_New.value(x + 1, y - 1)[8] = this->fi_Old.value(x, y)[8];
			}
		}
	}

	// now fi_New contains the streamed and collided values
}

void LatticeBoltzmann::Output(LatticeBoltzmannOutput &output)
{
	// Calculate macroscopic quantities for the output
	for (int y = 0; y < this->level.height; y++) {
		for (int x = 0; x < this->level.width; x++) {
			float &density = output.density.value(x, y);
			density = 0;
			const FICell &fi = fi_Old.value(x, y);
			for (int i = 0; i < 9; i++) {
				density += fi[i]; // density
			}
			glm::vec2& velocity = output.velocity.value(x, y);
			velocity[0] = fi[1] - fi[3] + fi[5] - fi[6] - fi[7] + fi[8]; // x Velocity
			velocity[1] = fi[2] - fi[4] + fi[5] + fi[6] - fi[7] - fi[8]; // y Velocity

		}
	}

	// Set fi_old = fi_new
	this->reinitilizeFI(output);
}

void LatticeBoltzmann::reinitilizeFI(LatticeBoltzmannOutput &output)
{
	output.prestream = fi_Old;
	output.afterstream = fi_New;
	fi_Old = fi_New;
}

float LatticeBoltzmann::handleWindShadow(const LatticeBoltzmannInput &input, int x, int y) {
	// this happens when a rigid body moves and an empty cell remains.
	//TODO: reinitialize density and everything else to something proper!
	float rho = 1.;
	return rho;
}

void LatticeBoltzmann::handleCollisions(const LatticeBoltzmannInput &input)
{
	for (int y = 1; y < level.height - 1; ++y) {
		for (int x = 1; x < level.width - 1; ++x) {
			// check for boundary
			if (input.flagfield.value(x, y) == Level::FLUID) {

				float rho = 0.0;
				float velx = 0.0;
				float vely = 0.0;

				// calculate density
				rho = fi_Old.value(x, y)[0] + fi_Old.value(x, y)[1] + fi_Old.value(x, y)[2] +
					  fi_Old.value(x, y)[3] + fi_Old.value(x, y)[4] + fi_Old.value(x, y)[5] +
					  fi_Old.value(x, y)[6] + fi_Old.value(x, y)[7] + fi_Old.value(x, y)[8];

				if (rho == 0.) {
					// this happens when a rigid body moves and an empty cell remains.
					rho = handleWindShadow(input, x, y);
				}

				const float rhoinv = 1.0 / rho;

				velx =
						((fi_Old.value(x, y)[5] + fi_Old.value(x, y)[1] + fi_Old.value(x, y)[8]) -
						 (fi_Old.value(x, y)[6] + fi_Old.value(x, y)[3] + fi_Old.value(x, y)[7])) *
						rhoinv;

				vely =
						((fi_Old.value(x, y)[6] + fi_Old.value(x, y)[2] + fi_Old.value(x, y)[5]) -
						 (fi_Old.value(x, y)[7] + fi_Old.value(x, y)[4] + fi_Old.value(x, y)[8])) *
						rhoinv;

				const float velxx = velx * velx;
				const float velyy = vely * vely;

				float omega = 0.6;
				// For the center
				float feqC = w[0] * rho * (1.0 - 3.0 * (velxx + velyy) * 0.5);
				fi_Old.value(x, y)[0] += omega * (feqC - fi_Old.value(x, y)[0]);

				float ax = 0;
				float ay = 0;
				// For the east distribution function
				float feqE =
						w[1] * rho * (1.0 + 3.0 * velx + 4.5 * velxx - 1.5 * (velxx + velyy));
				fi_Old.value(x, y)[1] +=
						omega * (feqE - fi_Old.value(x, y)[1]) + 3.0 * w[1] * rho * ax;

				// for the North East Direction
				float feqNE =
						w[5] * rho * (1.0 + 3.0 * (velx + vely) +
									  4.5 * (velx + vely) * (velx + vely) - 1.5 * (velxx + velyy));
				fi_Old.value(x, y)[5] +=
						omega * (feqNE - fi_Old.value(x, y)[5]) + 3.0 * w[1] * rho * (ax + ay);

				// For the north direction
				float feqN =
						w[2] * rho * (1.0 + 3.0 * vely + 4.5 * velyy - 1.5 * (velxx + velyy));
				fi_Old.value(x, y)[2] +=
						omega * (feqN - fi_Old.value(x, y)[2]) + 3.0 * w[1] * rho * ay;

				// For the North west direction
				float feqNW = w[6] * rho *
							  (1.0 + 3.0 * (-velx + vely) +
							   4.5 * (-velx + vely) * (-velx + vely) - 1.5 * (velxx + velyy));
				fi_Old.value(x, y)[6] +=
						omega * (feqNW - fi_Old.value(x, y)[6]) + 3.0 * w[1] * rho * (-ax + ay);

				// for the west direction
				float feqW =
						w[3] * rho * (1.0 - 3.0 * velx + 4.5 * velxx - 1.5 * (velxx + velyy));
				fi_Old.value(x, y)[3] +=
						omega * (feqW - fi_Old.value(x, y)[3]) + 3.0 * w[1] * rho * (-ax);

				// for the south west
				float feqSW =
						w[7] * rho * (1.0 - 3.0 * (velx + vely) +
									  4.5 * (velx + vely) * (velx + vely) - 1.5 * (velxx + velyy));
				fi_Old.value(x, y)[7] +=
						omega * (feqSW - fi_Old.value(x, y)[7]) + 3.0 * w[1] * rho * (-ax - ay);

				// for the South direction
				float feqS =
						w[4] * rho * (1.0 - 3.0 * vely + 4.5 * velyy - 1.5 * (velxx + velyy));
				fi_Old.value(x, y)[4] +=
						omega * (feqS - fi_Old.value(x, y)[4]) + 3.0 * w[1] * rho * (-ay);

				// for the south east direction
				float feqSE =
						w[8] * rho * (1.0 + 3.0 * (velx - vely) +
									  4.5 * (velx - vely) * (velx - vely) - 1.5 * (velxx + velyy));
				fi_Old.value(x, y)[8] +=
						omega * (feqSE - fi_Old.value(x, y)[8]) + 3.0 * w[1] * rho * (ax - ay);
			}
		}
	}
}

bool LatticeBoltzmann::isBoundaryValid(const Array2D<Level::CellType> &flagfield)
{
	int width = flagfield.width();
	int height = flagfield.height();
	bool result = true;
	for (int i = 0; i < width; i++) {  // top and bottom
		if (flagfield.value(i, 0) == Level::FLUID) result = false;
		if (flagfield.value(i, height - 1) == Level::FLUID) result = false;
	}
	for (int j = 1; j < height - 1; j++) {  // left and right sides
		if (flagfield.value(0, j) == Level::FLUID) result = false;
		if (flagfield.value(width - 1, j) == Level::FLUID) result = false;
	}
	if (!result) {
		std::cout << "FLAGS:" << std::endl << std::endl;
		int width = flagfield.width();
		int height = flagfield.height();

		for (int j = height - 1; j >= 0; j--) {
			for (int i = 0; i < width; i++) {
				std::cout << flagfield.value(i, j) << " ";
			}
			std::cout << std::endl;
		}
	}
	return result;
}

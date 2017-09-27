#ifndef RIGID_BODY_PHYSICS_INPUT_HPP_
#define RIGID_BODY_PHYSICS_INPUT_HPP_

#include <unordered_map>

#include "../LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "../../ext/glm/vec2.hpp"

class RigidBodyPhysicsInput {
private:
	const Array2D<FICell>* afterStream;
	const Array2D<FICell>* preStream;
	const int * cx;
	const int * cy;
	int invI[9] = {0,3,4,1,2,7,8,5,6};
public:
	float leftAngle, rightAngle;
    RigidBodyPhysicsInput() : afterStream(nullptr), preStream(nullptr),
	cx(nullptr), cy(nullptr), leftAngle(0), rightAngle(0) {}

	void process(const UserInputOutput &userInputOutput, const LatticeBoltzmannOutput &latticeBoltzmannOutput)
	{	
		afterStream = &latticeBoltzmannOutput.afterstream;
		preStream = &latticeBoltzmannOutput.prestream;
		cx = latticeBoltzmannOutput.cx;
		cy = latticeBoltzmannOutput.cy;
		leftAngle = userInputOutput.leftAngle[0];
		rightAngle = userInputOutput.rightAngle[0];
	}

	void computeImpulses(const Array2D<Level::CellType>& grid_ball,
		std::unordered_map<int, glm::vec2>& impulses) const {
		glm::vec2 impulse(0.0f,0.0f);
		int nbhX, nbhY;
		// TODO: Iterate over entire grid. For now do not iterate over the borders
		// TODO: Only one ball for now
		// TODO: Scale impulses properly
		for (int y = 1; y < grid_ball.height()-1; ++y) {
			for (int x = 1; x < grid_ball.width()-1; ++x) {
				if (grid_ball.value(x,y) != Level::CellType::FLUID)	{ // Go over only the obstacle cells
					for (int i = 1; i < 9; ++i) { // Go over the nbhrs
						nbhX = x + cx[i];
						nbhY = y + cy[i];
						impulse.x = (-1.0f*cx[i])*(afterStream->value(nbhX,nbhY)[invI[i]]+preStream->value(nbhX,nbhY)[invI[i]]); // Needs inverse direction
						impulse.y = (-1.0f*cy[i])*(afterStream->value(nbhX,nbhY)[invI[i]]+preStream->value(nbhX,nbhY)[invI[i]]); // Needs inverse direction
						impulses[grid_ball.value(x,y)] += (grid_ball.value(x+cx[i],y+cy[i]) == Level::CellType::FLUID? 1.0f:0.0f)*impulse; // If nbhr is fluid accumulate impulse
					}
				}
			}
		}
		// Debug
		// for (auto iter = impulses.begin(); iter != impulses.end(); ++iter) {
		// 	std::cout << iter->first << '\t' << iter->second.x << ' ' << iter->second.y << "\n";
		// }
	}

	~RigidBodyPhysicsInput() {
		afterStream = nullptr;
		preStream = nullptr;
	}

};

#endif

#ifndef RIGID_BODY_PHYSICS_INPUT_HPP_
#define RIGID_BODY_PHYSICS_INPUT_HPP_

#include <array>
#include <unordered_map>

#include "LatticeBoltzmann/LatticeBoltzmann.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "glm/vec2.hpp"

class RigidBodyPhysicsInput {
private:
	const Array2D<FICell>* afterStream;
	const Array2D<FICell>* preStream;

	static const int invI[9];
public:
	float leftAngle, rightAngle;
    RigidBodyPhysicsInput() : afterStream(nullptr), preStream(nullptr),
	leftAngle(0), rightAngle(0) {}

	bool isValid() const
	{
		return afterStream->width() && afterStream->height() && preStream->width() && preStream->height();
	}

	void process(const UserInputOutput &userInputOutput, const LatticeBoltzmannOutput &latticeBoltzmannOutput)
	{	
		afterStream = &latticeBoltzmannOutput.afterstream;
		preStream = &latticeBoltzmannOutput.prestream;
		leftAngle = userInputOutput.leftAngle[0];
		rightAngle = userInputOutput.rightAngle[0];
	}

	void computeImpulses(const Array2D<int>& grid_ball,
		std::unordered_map<int, glm::vec2>& impulses) const {
		glm::vec2 impulse(0.0f,0.0f);
		int nbhX, nbhY;
		// TODO: Iterate over entire grid. For now do not iterate over the borders
		// TODO: Only one ball for now
		// TODO: Scale impulses properly
		const float forceScaling = 1.;
		for (int y = 1; y < grid_ball.height()-1; ++y) {
			for (int x = 1; x < grid_ball.width()-1; ++x) {
				if (grid_ball.value(x,y) != 0)	{ // Go over only the obstacle cells
					for (int i = 1; i < 9; ++i) { // Go over the nbhrs
						nbhX = x + LatticeBoltzmann::cx[i];
						nbhY = y + LatticeBoltzmann::cy[i];
						impulse.x = forceScaling*(-1.0f*LatticeBoltzmann::cx[i])*(afterStream->value(nbhX,nbhY)[invI[i]]+preStream->value(nbhX,nbhY)[invI[i]]); // Needs inverse direction
						impulse.y = forceScaling*(-1.0f*LatticeBoltzmann::cy[i])*(afterStream->value(nbhX,nbhY)[invI[i]]+preStream->value(nbhX,nbhY)[invI[i]]); // Needs inverse direction
						impulses[grid_ball.value(x,y)] += (grid_ball.value(x+ LatticeBoltzmann::cx[i],y+ LatticeBoltzmann::cy[i]) == Level::CellType::FLUID? 1.0f:0.0f)*impulse; // If nbhr is fluid accumulate impulse
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

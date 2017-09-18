#ifndef CDATARIGIDBODY_HPP_
#define CDATARIGIDBODY_HPP_

#include <typeinfo>
#include <string.h>
#include "CPipelinePacket.hpp"

class CDataRigidBody : public CPipelinePacket {
private:
	std::vector<float> pos(2);
	std::vector<float> vel(2);
	float theta;
	float angVel;
	float mass;

public:
	CDataRigidBody(): pos(0.0,0.0), vel(0.0,0.0),
	theta(0.0), angVel(0.0), mass(1.0) {}

	CDataRigidBody(const std::vector<float>& p, const std::vector<float>& v,
		const float& t, const float& a, const float& m) :
	pos(p), vel(v), theta(t), angVel(a), mass(m) {}

	void *getPayloadRaw() {
		return this;
	}

	virtual void displayRigidBody() {
		std::cout << "-------------------------" << '\n';
		std::cout << "Position " << this->pos << '\n';
		std::cout << "Velocity " << this->vel << '\n';
		std::cout << "Orientation " << this->theta << '\n';
		std::cout << "Angular velocity" << this->angVel << '\n';
		std::cout << "Mass " << mass << '\n';
	}

	virtual ~CDataRigidBody();
};

class CDataCircle : public CDataRigidBody {
private:
	float radius;

public:

	CDataCircle(): radius(1.0), CDataRigidBody() {
		std::cout << "Creating a rigid circle with radius " << radius << '\n';
	}

	CDataCircle(const float& r): CDataRigidBody(), radius(r) {
		std::cout << "Creating a rigid circle with radius " << radius << '\n';
	}

	CDataCircle(const std::vector<float>& p, const std::vector<float>& v,
		const float& t, const float& a, const float& m, const float& r) :
	CDataRigidBody(p,v,t,a,m), radius(r) {
		std::cout << "Creating a rigid circle with radius " << radius << '\n';
	}

	float getRadius() {
		return this->radius;
	}

	void displayRigidBody() {
		CDataRigidBody::displayRigidBody();
		std::cout << "Radius " << radius << '\n';
		std::cout << "-------------------------" << '\n';
	}

	~CDataCircle();
};

#endif // CDATARIGIDBODY_HPP_

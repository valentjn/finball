#ifndef CDATARIGIDBODY_HPP_
#define CDATARIGIDBODY_HPP_

#include <typeinfo>
#include <string>
#include <vector>
#include "CPipelinePacket.hpp"

class CDataRigidBody {
private:
	std::vector<float> pos;
	std::vector<float> vel;
	float theta;
	float angVel;
	float mass;

public:
	CDataRigidBody(): pos(0.0, 0.0), vel(0.0, 0.0),
	theta(0.0), angVel(0.0), mass(1.0) {}

	CDataRigidBody(const std::vector<float>& p, const std::vector<float>& v,
		const float& t, const float& a, const float& m) :
	pos(p), vel(v), theta(t), angVel(a), mass(m) {}

	std::vector<float>& getPosition() {
		return pos;
	}

	void setPosition(const std::vector<float>& p) {
		pos.at(0) = p.at(0);
		pos.at(1) = p.at(1);
	}

	std::vector<float>& getVelocity() {
		return vel;
	}

	void setVelocity(const std::vector<float>& v) {
		vel.at(0) = v.at(0);
		vel.at(1) = v.at(1);
	}

	float getOrientation() {
		return theta;
	}

	void setOrientation(const float& t) {
		this->theta = t;
	}

	float getAngVelocity() {
		return angVel;
	}

	void setAngVelocity(const float& o) {
		this->angVel = o;
	}

	float getMass() {
		return mass;
	}

	void setMass(const float& m) {
		this->mass = m;
	}

	virtual void displayRigidBody() {
		std::cout << "-------------------------" << '\n';
		std::cout << "Position " << this->pos[0] << ',' << this->pos[1] << '\n';
		std::cout << "Velocity " << this->vel[0] << ',' << this->vel[1] << '\n';
		std::cout << "Orientation " << this->theta << '\n';
		std::cout << "Angular velocity " << this->angVel << '\n';
		std::cout << "Mass " << mass << '\n';
	}

	virtual ~CDataRigidBody() {}
};

class CDataCircle : public CDataRigidBody {
private:
	float radius;

public:
	CDataCircle(): CDataRigidBody(), radius(1.0) {}

	CDataCircle(const float& r): CDataRigidBody(), radius(r) {}

	CDataCircle(const std::vector<float>& p, const std::vector<float>& v,
		const float& t, const float& a, const float& m, const float& r) :
	CDataRigidBody(p,v,t,a,m), radius(r) {}

	float getRadius() {
		return this->radius;
	}

	void setRadius(const float& r) {
		this->radius = r;
	}

	virtual void displayRigidBody() {
		CDataRigidBody::displayRigidBody();
		std::cout << "Radius " << radius << '\n';
		std::cout << "-------------------------" << '\n';
	}

	~CDataCircle() {}
};

class CDataRigidBodyList : public CPipelinePacket {
public:
	std::vector<CDataRigidBody*> list;

	CDataRigidBodyList () {
		// TODO: Here we manually define for prototyping
		std::vector<float> p(1.0, 1.0);
		std::vector<float> v(2.0, 2.0);
		CDataCircle *c = new CDataCircle(p, v, 10.0, 15.0, 20.0, 25.0);
		list.push_back(c);
	}

	void displayList() {
		std::vector<CDataRigidBody*>::iterator it;
		for (it = list.begin(); it != list.end(); it++) {
			(*it)->displayRigidBody();
		}
	}

	~CDataRigidBodyList () {
		std::vector<CDataRigidBody*>::iterator it;
		for (it = list.begin(); it != list.end(); it++) {
			delete (*it);
		}
		list.clear();
	}
	void *getPayloadRaw() {
		return this;
	}
};

#endif // CDATARIGIDBODY_HPP_

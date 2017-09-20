#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include <typeinfo>
#include <string>
#include <vector>

#include "glm/vec2.hpp"

class RigidBody {
private:
	glm::vec2 pos;
	glm::vec2 vel;
	float theta;
	float angVel;
	float mass;
	int objType;

public:
	enum typeOfRigidBody
    {
        CIRCLE, RECTANGLE, TRIANGLE, ARBITRARY
    };

	RigidBody() :
        pos(2.f, 0.f),
        vel(2.f, 0.f),
	    theta(0.f),
        angVel(0.f),
        mass(1.f)
    {}

	RigidBody(const glm::vec2 &p,
              const glm::vec2 &v,
	          const float &t,
              const float &a,
              const float &m,
              const int &obj) :
	    pos(p),
        vel(v),
        theta(t),
        angVel(a),
        mass(m),
        objType(obj)
    {}

	glm::vec2 &getPosition()
    {
		return pos;
	}

	void setPosition(const glm::vec2 &p)
    {
		pos.x = p.x;
		pos.y = p.y;
	}

	glm::vec2 &getVelocity()
    {
		return vel;
	}

	void setVelocity(const glm::vec2 &v)
    {
		vel.x = v.x;
		vel.y = v.y;
	}

	float getOrientation()
	{
		return theta;
	}

	void setOrientation(const float &theta)
    {
		this->theta = theta;
	}

	float getAngVelocity()
	{
		return angVel;
	}

	void setAngVelocity(const float &angVel)
    {
		this->angVel = angVel;
	}

	float getMass()
	{
		return mass;
	}

	void setMass(const float &m)
    {
		mass = m;
	}

	int getType()
	{
		return objType;
	}

	void setType(typeOfRigidBody t)
    {
		objType = t;
	}

	virtual void displayRigidBody()
	{
		std::cout << "-------------------------" << std::endl;
		std::cout << "Position " << pos.x << ',' << pos.y << std::endl;
		std::cout << "Velocity " << vel.x << ',' << vel.y << std::endl;
		std::cout << "Orientation " << theta << std::endl;
		std::cout << "Angular velocity " << angVel << std::endl;
		std::cout << "Mass " << mass << std::endl;
	}

	virtual ~RigidBody() {}
};

class RigidBodyCircle : public RigidBody {
private:
	float radius;

public:
	RigidBodyCircle() : RigidBodyCircle(1.f)

	RigidBodyCircle(const float &r) : RigidBody(), radius(r)
    {
		setType(CIRCLE);
	}

	RigidBodyCircle(const glm::vec2 &p,
                    const glm::vec2 &v,
		            const float &t,
                    const float &a,
                    const float &m,
                    const float &r) :
        RigidBody(p, v, t, a, m, CIRCLE), radius(r)
    {}

	float getRadius()
	{
		return radius;
	}

	void setRadius(const float &r)
    {
		radius = r;
	}

	virtual void displayRigidBody()
	{
		RigidBody::displayRigidBody();
		std::cout << "Radius " << radius << std::endl;
		std::cout << "-------------------------" << std::endl;
	}

	~RigidBodyCircle() {}
};

class RigidBodyList {
public:
	std::vector<RigidBody*> list;

	RigidBodyList ()
    {
        // TODO: remove test data

		glm::vec2 p(1.f, 1.f);
		glm::vec2 v(2.f, 2.f);
		RigidBodyCircle *c = new RigidBodyCircle(p, v, 10.f, 15.f, 20.f, 25.f);
		list.push_back(c);
	}

	void displayList()
	{
		std::vector<RigidBody*>::iterator it;
		for (it = list.begin(); it != list.end(); it++) {
			(*it)->displayRigidBody();
		}
	}

	size_t getSize()
    {
		return list.size();
	}

	RigidBody *getRigidBody(const int &idx)
    {
		return list.at(idx);
	}

	~RigidBodyList ()
    {
		std::vector<RigidBody*>::iterator it;
		for (it = list.begin(); it != list.end(); it++) {
			delete (*it);
		}
		list.clear();
	}
};

#endif // RIGID_BODY_HPP_

#include "physics.h"
#include <cmath>

#include <iostream>

PhysicsVector::PhysicsVector(double x, double y)
{
  this->x = x;
  this->y = y;
};

PhysicsVector PhysicsVector::operator+(PhysicsVector const &obj)
{
  double newX = obj.x + this->x;
  double newY = obj.y + this->y;
  return PhysicsVector(newX, newY);
};

PhysicsVector PhysicsVector::operator-(PhysicsVector const &obj)
{
  double newX = this->x - obj.x;
  double newY = this->y - obj.y;
  return PhysicsVector(newX, newY);
}

PhysicsVector PhysicsVector::operator*(PhysicsVector const &obj)
{
  double dProd = this->x * obj.x + this->y * obj.y;
  return dProd;
}

PhysicsVector PhysicsVector::smul(double k)
{
  return PhysicsVector(this->x * k, this->y * k);
}

PhysicsVector PhysicsVector::sdiv(double k)
{
  return PhysicsVector(this->x / k, this->y / k);
}

PhysicsVector PhysicsVector::norm()
{
  double length = sqrt(std::pow(this->x, 2) + std::pow(this->y, 2));
  return PhysicsVector(this->x / length, this->y / length);
}

PhysicsObject::PhysicsObject(double mass, double size, PhysicsVector position, PhysicsVector velocity)
{
  this->mass = mass;
  this->size = size;
  this->position = position;
  this->velocity = velocity;
}

void PhysicsObject::applyForce(PhysicsVector force)
{
  this->forces.push_back(force);
}

void PhysicsObject::tick(double timeDelta)
{

  PhysicsVector netForce = PhysicsVector(0, 0);
  for (PhysicsVector force : this->forces)
  {
    netForce = netForce + force;
  }
  this->forces.clear();

  PhysicsVector acceleration = netForce.sdiv(this->mass).smul(timeDelta);
  this->velocity = this->velocity + acceleration;
  this->position = this->position + this->velocity.smul(timeDelta);
}

// helper function 1D collision
std::pair<double, double> collide1D(double m1, double v1, double m2, double v2)
{
  double v1f = (v1 * (m1 - m2) + 2 * m2 * v2) / (m1 + m2);
  double v2f = (v2 * (m2 - m1) + 2 * m1 * v1) / (m1 + m2);
  return std::make_pair(v1f, v2f);
}

bool PhysicsObject::isColliding(PhysicsObject &obj)
{
  // for this we assume all objects are perfect circles
  double dist = sqrt(pow(this->position.x - obj.position.x, 2) + pow(this->position.y - obj.position.y, 2));

  return dist <= this->size + obj.size;
}

void PhysicsObject::collide(PhysicsObject &obj)
{
  // check if they are inside each other
  double dist = sqrt(pow(this->position.x - obj.position.x, 2) + pow(this->position.y - obj.position.y, 2));
  double overlap = this->size + obj.size - dist;

  // if the overlap is more than 0.1m, then apply a strong force to separate them
  if (overlap > 0.1)
  {
    // calculate the direction of the force
    PhysicsVector direction = this->position - obj.position;
    direction = direction.sdiv(dist);

    // get the norm of the direction
    PhysicsVector norm = direction.norm();

    // apply the force
    double strength = overlap;
    this->applyForce(norm.smul(strength * 10));
    obj.applyForce(norm.smul(strength * -10));

    // teleport them away
    this->position = this->position + norm.smul(overlap * 0.5);
    obj.position = obj.position - norm.smul(overlap * 0.5);
  }

  // for now, make it a perfectly elastic collision
  // https://en.wikipedia.org/wiki/Elastic_collision#One-dimensional_Newtonian

  auto velsX = collide1D(this->mass, this->velocity.x, obj.mass, obj.velocity.x);
  auto velsY = collide1D(this->mass, this->velocity.y, obj.mass, obj.velocity.y);

  this->velocity.x = velsX.first;
  obj.velocity.x = velsX.second;

  this->velocity.y = velsY.first;
  obj.velocity.y = velsY.second;
}
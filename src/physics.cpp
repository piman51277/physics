#include "physics.h"
#include <cmath>

#include <iostream>

PhysicsVector::PhysicsVector(float x, float y)
{
  this->x = x;
  this->y = y;
};

PhysicsVector PhysicsVector::operator+(PhysicsVector const &obj)
{
  float newX = obj.x + this->x;
  float newY = obj.y + this->y;
  return PhysicsVector(newX, newY);
};

PhysicsVector PhysicsVector::operator-(PhysicsVector const &obj)
{
  float newX = this->x - obj.x;
  float newY = this->y - obj.y;
  return PhysicsVector(newX, newY);
}

float PhysicsVector::operator*(PhysicsVector const &obj)
{
  float dProd = this->x * obj.x + this->y * obj.y;
  return dProd;
}

PhysicsVector PhysicsVector::smul(float k)
{
  return PhysicsVector(this->x * k, this->y * k);
}

PhysicsVector PhysicsVector::sdiv(float k)
{
  return PhysicsVector(this->x / k, this->y / k);
}

float PhysicsVector::magnitude()
{
  return sqrt(std::pow(this->x, 2) + std::pow(this->y, 2));
}

PhysicsVector PhysicsVector::norm()
{
  float length = this->magnitude();
  return PhysicsVector(this->x / length, this->y / length);
}

PhysicsVector PhysicsVector::orthogonal()
{
  PhysicsVector orth(-1.0 / this->x, 1.0 / this->y);
  float currMag = this->magnitude();
  return orth.norm().smul(currMag);
}

PhysicsVector PhysicsVector::project(PhysicsVector v)
{
  PhysicsVector norm = v.norm();
  PhysicsVector tvec(this->x, this->y);
  return norm.smul(norm * tvec);
}

// Uses an orthogonal basis to find the coordinates of a vector in that basis
PhysicsVector PhysicsVector::useBasis(PhysicsVector v, PhysicsVector basis1, PhysicsVector basis2)
{
  // first find the coordinates of v in the basis
  float x = v.project(basis1).magnitude();
  float y = v.project(basis2).magnitude();

  return PhysicsVector(x, y);
}

PhysicsObject::PhysicsObject(float mass, float size, PhysicsVector position, PhysicsVector velocity)
{
  this->mass = mass;
  this->size = size;
  this->position = position;
  this->velocity = velocity;
}

void PhysicsObject::applyForce(PhysicsVector force)
{
  this->netForce = this->netForce + force;
}

void PhysicsObject::tick(float timeDelta)
{
  PhysicsVector acceleration = netForce.sdiv(this->mass).smul(timeDelta);
  netForce = {0, 0};
  this->velocity = this->velocity + acceleration;
  this->position = this->position + this->velocity.smul(timeDelta);
}

// helper function 1D collision
std::pair<float, float> collide1D(float m1, float v1, float m2, float v2)
{
  float v1f = (v1 * (m1 - m2) + 2 * m2 * v2) / (m1 + m2);
  float v2f = (v2 * (m2 - m1) + 2 * m1 * v1) / (m1 + m2);
  return std::make_pair(v1f, v2f);
}

bool PhysicsObject::isBoxColliding(PhysicsObject &obj)
{
  // bounding box check
  if (this->position.x + this->size < obj.position.x - obj.size)
  {
    return false;
  }
  if (this->position.x - this->size > obj.position.x + obj.size)
  {
    return false;
  }
  if (this->position.y + this->size < obj.position.y - obj.size)
  {
    return false;
  }
  if (this->position.y - this->size > obj.position.y + obj.size)
  {
    return false;
  }

  return true;
}

void PhysicsObject::collide(PhysicsObject &obj)
{
  // check if they are inside each other
  float dist = sqrt(pow(this->position.x - obj.position.x, 2) + pow(this->position.y - obj.position.y, 2));
  float overlap = this->size + obj.size - dist;

  // if they are not overlapping, then don't do anything
  if (overlap <= 0)
  {
    return;
  }

  PhysicsVector direction = (this->position - obj.position).norm();

  // if the overlap is more than 0.1m, then separate them
  if (overlap > 0.1)
  {
    // teleport them away
    this->position = this->position + direction.smul(overlap * 0.5);
    obj.position = obj.position - direction.smul(overlap * 0.5);
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
#pragma once
#include <vector>

class PhysicsVector
{
public:
  float x;
  float y;

  PhysicsVector(float x = 0, float y = 0);

  // vector operaions
  PhysicsVector operator+(PhysicsVector const &obj);
  PhysicsVector operator-(PhysicsVector const &obj);

  // dot product
  float operator*(PhysicsVector const &obj);

  // scalar operations
  PhysicsVector smul(float k);
  PhysicsVector sdiv(float k);

  PhysicsVector norm();
  float magnitude();

  PhysicsVector project(PhysicsVector onto);

  static PhysicsVector useBasis(PhysicsVector v, PhysicsVector basis1, PhysicsVector basis2);

  PhysicsVector orthogonal();
};

class PhysicsObject
{
public:
  PhysicsVector position;
  PhysicsVector velocity;
  float mass;
  float size;
  PhysicsVector netForce;

  PhysicsObject(float mass = 0.0, float size = 1.0, PhysicsVector position = {0, 0}, PhysicsVector velocity = {0, 0});

  void applyForce(PhysicsVector force);

  void tick(float timeDelta);

  bool isBoxColliding(PhysicsObject &obj);

  // This should be called *once* per pair!
  void collide(PhysicsObject &obj);
};
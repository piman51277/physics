#pragma once
#include <vector>

class PhysicsVector
{
public:
  double x;
  double y;

  PhysicsVector(double x = 0, double y = 0);

  // vector operaions
  PhysicsVector operator+(PhysicsVector const &obj);
  PhysicsVector operator-(PhysicsVector const &obj);

  // dot product
  double operator*(PhysicsVector const &obj);

  // scalar operations
  PhysicsVector smul(double k);
  PhysicsVector sdiv(double k);

  PhysicsVector norm();
  double magnitude();

  PhysicsVector project(PhysicsVector onto);

  static PhysicsVector useBasis(PhysicsVector v, PhysicsVector basis1, PhysicsVector basis2);

  PhysicsVector orthogonal();
};

class PhysicsObject
{
public:
  PhysicsVector position;
  PhysicsVector velocity;
  double mass;
  double size;
  std::vector<PhysicsVector> forces = {};

  PhysicsObject(double mass = 0.0, double size = 1.0, PhysicsVector position = {0, 0}, PhysicsVector velocity = {0, 0});

  void applyForce(PhysicsVector force);

  void tick(double timeDelta);

  bool isColliding(PhysicsObject &obj);

  // This should be called *once* per pair!
  void collide(PhysicsObject &obj);
};
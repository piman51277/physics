#pragma once
#include "physics.h"
#include <vector>

class Sim
{
public:
  std::pair<double, double> xBounds;
  std::pair<double, double> yBounds;

  double wallRestitution = 0.8;

  std::vector<PhysicsObject> objects = {};

  Sim(std::pair<double, double> xBounds, std::pair<double, double> yBounds);

  void addObject(PhysicsObject obj);

  // for applying user-controlled forces. Called first
  void demoTick(double timeDelta);

  // applies all physics to all objects. Called last
  void physicsTick(double timeDelta);

  // performs a full tick
  void tick(double timeDelta);
};
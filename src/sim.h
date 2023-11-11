#pragma once
#include "physics.h"
#include <vector>

class Sim
{
public:
  std::pair<float, float> xBounds;
  std::pair<float, float> yBounds;

  float wallRestitution = 1.0;

  std::vector<PhysicsObject> objects = {};

  Sim(std::pair<float, float> xBounds, std::pair<float, float> yBounds);

  void addObject(PhysicsObject obj);

  // for applying user-controlled forces. Called first
  void demoTick(float timeDelta);

  // applies all physics to all objects. Called last
  void physicsTick(float timeDelta);

  // performs a full tick
  void tick(float timeDelta);
};
#include "sim.h"
#include <iostream>

Sim::Sim(std::pair<double, double> xBounds, std::pair<double, double> yBounds)
{
  this->xBounds = xBounds;
  this->yBounds = yBounds;
}

void Sim::addObject(PhysicsObject obj)
{
  // cap entity count at 6000
  if (this->objects.size() >= 6000)
  {
    return;
  }

  this->objects.push_back(obj);
}

void Sim::physicsTick(double timeDelta)
{

  // first, for every pair of objects, check if they are colliding
  for (int i = 0; i < this->objects.size(); i++)
  {
    for (int j = i + 1; j < this->objects.size(); j++)
    {
      if (this->objects[i].isBoxColliding(this->objects[j]))
      {
        this->objects[i].collide(this->objects[j]);
      }
    }
  }

  // then, process each object
  for (int i = 0; i < this->objects.size(); i++)
  {
    // tick
    this->objects[i].tick(timeDelta);

    // check bounds
    if (this->objects[i].position.x < this->xBounds.first)
    {
      this->objects[i].position.x = this->xBounds.first;
      this->objects[i].velocity.x = -this->objects[i].velocity.x * wallRestitution;
    }
    else if (this->objects[i].position.x > this->xBounds.second)
    {
      this->objects[i].position.x = this->xBounds.second;
      this->objects[i].velocity.x = -this->objects[i].velocity.x * wallRestitution;
    }

    if (this->objects[i].position.y < this->yBounds.first)
    {
      this->objects[i].position.y = this->yBounds.first;
      this->objects[i].velocity.y = -this->objects[i].velocity.y * wallRestitution;
    }
    else if (this->objects[i].position.y > this->yBounds.second)
    {
      this->objects[i].position.y = this->yBounds.second;
      this->objects[i].velocity.y = -this->objects[i].velocity.y * wallRestitution;
    }
  }
}

void Sim::demoTick(double timeDelta)
{
  return;
}

void Sim::tick(double timeDelta)
{
  this->demoTick(timeDelta);
  this->physicsTick(timeDelta);
}
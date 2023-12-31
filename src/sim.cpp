#include "sim.h"
#include <iostream>

Sim::Sim(std::pair<float, float> xBounds, std::pair<float, float> yBounds)
{
  this->xBounds = xBounds;
  this->yBounds = yBounds;

  this->objects.reserve(10000);
}

void Sim::addObject(PhysicsObject obj)
{
  // cap entity count at 10000
  if (this->objects.size() >= 10000)
  {
    return;
  }

  this->objects.push_back(obj);
}

void Sim::physicsTick(float timeDelta)
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

    // check bounds (accounting for size)
    if (this->objects[i].position.x + this->objects[i].size > this->xBounds.second)
    {
      this->objects[i].position.x = this->xBounds.second - this->objects[i].size;
      this->objects[i].velocity.x = -this->objects[i].velocity.x * this->wallRestitution;
    }
    else if (this->objects[i].position.x - this->objects[i].size < this->xBounds.first)
    {
      this->objects[i].position.x = this->xBounds.first + this->objects[i].size;
      this->objects[i].velocity.x = -this->objects[i].velocity.x * this->wallRestitution;
    }
    if (this->objects[i].position.y + this->objects[i].size > this->yBounds.second)
    {
      this->objects[i].position.y = this->yBounds.second - this->objects[i].size;
      this->objects[i].velocity.y = -this->objects[i].velocity.y * this->wallRestitution;
    }
    else if (this->objects[i].position.y - this->objects[i].size < this->yBounds.first)
    {
      this->objects[i].position.y = this->yBounds.first + this->objects[i].size;
      this->objects[i].velocity.y = -this->objects[i].velocity.y * this->wallRestitution;
    }
  }
}

void Sim::demoTick(float timeDelta)
{
  return;
}

void Sim::tick(float timeDelta)
{
  this->demoTick(timeDelta);
  this->physicsTick(timeDelta);
}
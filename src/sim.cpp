#include "sim.h"

Sim::Sim(std::pair<double, double> xBounds, std::pair<double, double> yBounds)
{
  this->xBounds = xBounds;
  this->yBounds = yBounds;
}

void Sim::addObject(PhysicsObject obj)
{
  this->objects.push_back(obj);
}

PhysicsVector Gravity = {0, 40};

void Sim::physicsTick(double timeDelta)
{
  // first, for every pair of objects, check if they are colliding
  for (int i = 0; i < this->objects.size(); i++)
  {
    for (int j = i + 1; j < this->objects.size(); j++)
    {
      if (this->objects[i].isColliding(this->objects[j]))
      {
        this->objects[i].collide(this->objects[j]);
      }
    }
  }

  const double restitution = 0.8;

  // then, process each object
  for (int i = 0; i < this->objects.size(); i++)
  {
    // apply gravity
    this->objects[i].applyForce(Gravity.smul(this->objects[i].mass));

    // tick
    this->objects[i].tick(timeDelta);

    // check bounds
    if (this->objects[i].position.x < this->xBounds.first)
    {
      this->objects[i].position.x = this->xBounds.first;
      this->objects[i].velocity.x = -this->objects[i].velocity.x * restitution;
    }
    else if (this->objects[i].position.x > this->xBounds.second)
    {
      this->objects[i].position.x = this->xBounds.second;
      this->objects[i].velocity.x = -this->objects[i].velocity.x * restitution;
    }

    if (this->objects[i].position.y < this->yBounds.first)
    {
      this->objects[i].position.y = this->yBounds.first;
      this->objects[i].velocity.y = -this->objects[i].velocity.y * restitution;
    }
    else if (this->objects[i].position.y > this->yBounds.second)
    {
      this->objects[i].position.y = this->yBounds.second;
      this->objects[i].velocity.y = -this->objects[i].velocity.y * restitution;
    }
  }
}

void Sim::demoTick(double timeDelta)
{
  // don't do anything
  return;
}

void Sim::tick(double timeDelta)
{
  this->demoTick(timeDelta);
  this->physicsTick(timeDelta);
}
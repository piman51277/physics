#pragma once
#include <SDL2/SDL.h>
#include "sim.h"
#include "physics.h"
#include <chrono>
#include <list>

class Frame
{

  SDL_Window *window;
  SDL_Renderer *renderer;
  Sim *sim;

  float scale;

  std::list<PhysicsVector> path = {};

  void draw();
  void tick(float timeDelta);

  void mouseDownHandler(SDL_MouseButtonEvent e);

public:
  Frame(int width, int height, const char *title, float scale = 10.0, int framerate = 60);
  ~Frame();
};
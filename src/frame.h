#pragma once
#include <SDL2/SDL.h>
#include "sim.h"
#include "physics.h"
#include <chrono>

class Frame
{

  SDL_Window *window;
  SDL_Renderer *renderer;
  Sim *sim;

  float scale;
  void draw();
  void tick(double timeDelta);

  void mouseDownHandler(SDL_MouseButtonEvent e);

public:
  Frame(int width, int height, const char *title, float scale = 10.0, int framerate = 60);
  ~Frame();
};
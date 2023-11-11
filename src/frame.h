#pragma once
#include <SDL2/SDL.h>
#include "sim.h"
#include "physics.h"
#include <chrono>

class Frame
{

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_TimerID drawTimer;
  SDL_TimerID tickTimer;
  Sim *sim;

  float scale;

  std::chrono::time_point<std::chrono::system_clock> lastTick;

  void draw();
  static Uint32 staticTick(Uint32 interval, void *param);

  void mouseDownHandler(SDL_MouseButtonEvent e);

public:
  Frame(int width, int height, const char *title, float scale = 10.0, int framerate = 60, int tickrate = 20);
  ~Frame();
};
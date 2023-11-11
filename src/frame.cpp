#include "frame.h"
#include <map>
#include <iostream>

Frame::Frame(int width, int height, const char *title, float scale, int framerate, int tickrate)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  this->scale = scale;

  std::pair<double, double> xBounds = {0, (double)width * scale};
  std::pair<double, double> yBounds = {0, (double)height * scale};

  sim = new Sim(xBounds, yBounds);

  int drawInterval = 1000.0 / (double)framerate;

  int tickInterval = 1000.0 / (double)tickrate;
  tickTimer = SDL_AddTimer(tickInterval, Frame::staticTick, this);

  auto lastFrame = std::chrono::system_clock::now();

  while (true)
  {
    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
      switch (e.type)
      {
      case SDL_QUIT:
        this->~Frame();
        return;
      case SDL_MOUSEBUTTONDOWN:
        mouseDownHandler(*reinterpret_cast<SDL_MouseButtonEvent *>(&e));
        break;
      case SDL_MOUSEMOTION:
        mouseDownHandler(*reinterpret_cast<SDL_MouseButtonEvent *>(&e));
        break;
      }
    }

    draw();

    auto current = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = current - lastFrame;
    double time = elapsed.count();

    if (time * 1000 < drawInterval)
    {
      SDL_Delay(drawInterval - time * 1000);
    }
  }
}

Frame::~Frame()
{
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}

void Frame::draw()
{
  constexpr float trig[3] = {
      0.923879, // cos(22.5), sin(67.5)
      0.707106, // cos(45), sin(45)
      0.382683  // cos(67.5), sin(22.5)
  };

  static std::map<int, SDL_FPoint *> offsetCache;

  Sim *sim = this->sim;
  std::vector<PhysicsObject> objects = sim->objects;

  // clear the screen
  SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
  SDL_RenderClear(this->renderer);

  // draw the objects onto the canvas
  for (PhysicsObject obj : objects)
  {
    PhysicsVector pos = obj.position;
    float size = obj.size;

    // drawing an actual circle is hard, so we will draw 16-gons instead
    SDL_FPoint points[17];
    SDL_FPoint *offsets = new SDL_FPoint[17];

    // do we already have the points for this size?
    int sizeId = (int)size * 1000;
    if (offsetCache.find(sizeId) != offsetCache.end())
    {
      // yes, we do
      offsets = offsetCache[sizeId];
    }
    else
    {
      // precompute offsets
      const float tbl[3] = {
          size * trig[0],
          size * trig[1],
          size * trig[2]};

      // going CC
      //(these are relative offsets, we will offset later)

      // Q1
      offsets[0] = {size, 0};
      offsets[1] = {tbl[0], tbl[2]};
      offsets[2] = {tbl[1], tbl[1]};
      offsets[3] = {tbl[2], tbl[0]};

      // Q2
      offsets[4] = {0, size};
      offsets[5] = {-tbl[2], tbl[0]};
      offsets[6] = {-tbl[1], tbl[1]};
      offsets[7] = {-tbl[0], tbl[2]};

      // Q3
      offsets[8] = {-size, 0};
      offsets[9] = {-tbl[0], -tbl[2]};
      offsets[10] = {-tbl[1], -tbl[1]};
      offsets[11] = {-tbl[2], -tbl[0]};

      // Q4
      offsets[12] = {0, -size};
      offsets[13] = {tbl[2], -tbl[0]};
      offsets[14] = {tbl[1], -tbl[1]};
      offsets[15] = {tbl[0], -tbl[2]};

      offsets[16] = {size, 0};

      offsetCache[sizeId] = offsets;
    }

    // scale & offset points
    for (int i = 0; i < 17; i++)
    {
      points[i].x = (offsets[i].x + pos.x) * (1.0 / scale);
      points[i].y = (offsets[i].y + pos.y) * (1.0 / scale);
    }

    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
    SDL_RenderDrawLinesF(this->renderer, points, 17);
  }

  SDL_RenderPresent(this->renderer);
}

Uint32 Frame::staticTick(Uint32 interval, void *param)
{
  Frame *self = static_cast<Frame *>(param);
  Sim *sim = self->sim;

  // get time since last tick in seconds
  auto current = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = current - self->lastTick;
  double time = elapsed.count();
  self->lastTick = current;

  sim->tick(time);

  return interval;
}

void Frame::mouseDownHandler(SDL_MouseButtonEvent e)
{
  if (e.button == SDL_BUTTON_LEFT)
  {
    PhysicsVector pos = {(double)e.x * scale, (double)e.y * scale};
    PhysicsObject obj = {1, 10, pos, {0, 0}};
    sim->addObject(obj);
  }
}
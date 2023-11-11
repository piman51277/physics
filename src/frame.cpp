#include "frame.h"
Frame::Frame(int width, int height, const char *title, int framerate, int tickrate)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  std::pair<double, double> xBounds = {0, (double)width * 2.0};
  std::pair<double, double> yBounds = {0, (double)height * 2.0};

  sim = new Sim(xBounds, yBounds);

  int drawInterval = 1000.0 / (double)framerate;

  int tickInterval = 1000.0 / (double)tickrate;
  tickTimer = SDL_AddTimer(tickInterval, Frame::staticTick, this);

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
      }
    }

    draw();

    SDL_Delay(drawInterval);
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

    // precompute points
    const float tbl[3] = {
        size * trig[0],
        size * trig[1],
        size * trig[2]};

    // going CC
    //(these are relative points, we will offset later)

    // Q1
    points[0] = {size, 0};
    points[1] = {tbl[0], tbl[2]};
    points[2] = {tbl[1], tbl[1]};
    points[3] = {tbl[2], tbl[0]};

    // Q2
    points[4] = {0, size};
    points[5] = {-tbl[2], tbl[0]};
    points[6] = {-tbl[1], tbl[1]};
    points[7] = {-tbl[0], tbl[2]};

    // Q3
    points[8] = {-size, 0};
    points[9] = {-tbl[0], -tbl[2]};
    points[10] = {-tbl[1], -tbl[1]};
    points[11] = {-tbl[2], -tbl[0]};

    // Q4
    points[12] = {0, -size};
    points[13] = {tbl[2], -tbl[0]};
    points[14] = {tbl[1], -tbl[1]};
    points[15] = {tbl[0], -tbl[2]};

    points[16] = {size, 0};

    // scale & offset points
    for (int i = 0; i < 17; i++)
    {
      points[i].x += pos.x;
      points[i].y += pos.y;

      points[i].x *= 0.5;
      points[i].y *= 0.5;
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
    PhysicsVector pos = {(double)e.x * 2.0, (double)e.y * 2.0};
    PhysicsObject obj = {1, 100, pos, {0, 0}};
    sim->addObject(obj);
  }
}
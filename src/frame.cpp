#include "frame.h"
#include <map>
#include <random>

Frame::Frame(int width, int height, const char *title, float scale, int framerate)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  this->scale = scale;

  std::pair<float, float> xBounds = {0, (float)width * scale};
  std::pair<float, float> yBounds = {0, (float)height * scale};

  sim = new Sim(xBounds, yBounds);

  int tickInterval = 1000.0 / (float)framerate;
  auto lastTick = std::chrono::system_clock::now();

  PhysicsObject lgObj(5, 50, {960, 540}, {0, 0});
  sim->addObject(lgObj);
  std::mt19937 rng;
  std::uniform_real_distribution<float> dist(-300, 300);

  // add objects in a grid
  for (int x = 10; x <= 1910; x += 30)
  {
    for (int y = 10; y <= 1070; y += 30)
    {
      PhysicsObject smObj(1, 1, {(float)x, (float)y}, {dist(rng), dist(rng)});

      if (!lgObj.isBoxColliding(smObj))
      {
        sim->addObject(smObj);
      }
    }
  }
  // wait until click to start
  while (true)
  {
    SDL_Event e;
    if (SDL_WaitEvent(&e))
    {
      if (e.type == SDL_MOUSEBUTTONDOWN)
      {
        lastTick = std::chrono::system_clock::now();
        break;
      }
    }
  }

  while (true)
  {
    auto current = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed = current - lastTick;
    float delta = elapsed.count() * 1000;
    lastTick = current;

    // process all the events that have come in between
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
      else
      {
        break;
      }
    }

    tick(delta / 1000);
    draw();

    if (delta <= tickInterval)
    {
      // inceased scheduling delay
      SDL_Delay(tickInterval - delta);
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

  // clear the screen
  SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
  SDL_RenderClear(this->renderer);

  // draw the objects onto the canvas
  for (PhysicsObject obj : sim->objects)
  {
    PhysicsVector pos = obj.position;
    float size = obj.size;

    // tickIng an actual circle is hard, so we will draw 16-gons instead
    SDL_FPoint points[17];
    SDL_FPoint *offsets = new SDL_FPoint[17];

    // do we already have the points for this size?
    int sizeId = (int)size * 1000;
    if (offsetCache.find(sizeId) != offsetCache.end())
    {
      // unallocate the previous offsets
      delete[] offsets;

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

  // draw the path
  SDL_FPoint fpath[10000];
  int i = 0;
  for (auto elem : this->path)
  {
    fpath[i] = {elem.x, elem.y};
    i += 1;
  }
  SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 255);
  SDL_RenderDrawLinesF(this->renderer, fpath, this->path.size());

  SDL_RenderPresent(this->renderer);
}

void Frame::tick(float timeDelta)
{
  this->sim->tick(timeDelta);

  // add the pos of the first element to path
  this->path.push_back(this->sim->objects[0].position);

  // if the path is longer than 1000, cull
  if (this->path.size() > 10000)
  {
    this->path.pop_front();
  }
}

void Frame::mouseDownHandler(SDL_MouseButtonEvent e)
{
  static std::mt19937 rng;
  static std::uniform_real_distribution<float> dist(-200, 200);

  if (e.button == SDL_BUTTON_LEFT)
  {
    PhysicsVector pos = {(float)e.x * scale, (float)e.y * scale};
    PhysicsObject obj = {1, 10, pos, {dist(rng), dist(rng)}};
    sim->addObject(obj);
  }
}
#pragma once

#include <claws/container/vect.hpp>

namespace state
{
  class GameState;
}

class Wasp;

class Gun
{
public:
  claws::vect<float, 2> position;
  claws::vect<float, 2> speed;
  float radius{0.05f};
  virtual ~Gun() noexcept = default;
  virtual bool fire(state::GameState &gameState, Wasp *wasp, claws::vect<float, 2u> position, claws::vect<float, 2u> dir) = 0;
  virtual void update() = 0;
  virtual float getHeat() = 0;
};

namespace guns
{
  Gun *makeNothing();
}

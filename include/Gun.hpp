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
  virtual ~Gun() noexcept = default;
  virtual void fire(state::GameState &gameState, Wasp *wasp, claws::vect<float, 2u> position, claws::vect<float, 2u> dir) = 0;
  virtual void update() = 0;
  virtual float getHeat() = 0;

};


namespace guns
{
  Gun *makeNothing();
}

#pragma once

#include <claws/container/vect.hpp>

namespace state
{
  class GameState;
}

class Gun
{
public:
  virtual ~Gun() noexcept = default;
  virtual void fire(state::GameState &gameState, claws::vect<float, 2u> position, claws::vect<float, 2u> dir) = 0;
};


namespace guns
{
  Gun *makeNothing();
}

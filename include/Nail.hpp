#pragma once

#include <claws/container/vect.hpp>

class Wasp;

class Nail
{
public:
  claws::vect<float, 2u> position;
  claws::vect<float, 2u> speed;
  Wasp *immune;
  uint32_t timer{180};

  Nail(claws::vect<float, 2u> position, claws::vect<float, 2u> speed, Wasp *immune) noexcept;
  ~Nail() noexcept;

  void update() noexcept;

  bool canBeRemoved() const noexcept
  {
    return !timer;
  }
};

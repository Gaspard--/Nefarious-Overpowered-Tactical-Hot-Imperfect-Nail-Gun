#pragma once

#include <claws/container/vect.hpp>

class Nail
{
public:
  claws::vect<float, 2u> position;
  claws::vect<float, 2u> speed;
  uint32_t timer{180};

  Nail(claws::vect<float, 2u> position, claws::vect<float, 2u> speed) noexcept;
  ~Nail() noexcept;

  void update() noexcept;

  bool canBeRemoved() const noexcept
  {
    return !timer;
  }
};

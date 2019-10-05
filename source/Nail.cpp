#include "Nail.hpp"

Nail::Nail(claws::vect<float, 2u> position,
	   claws::vect<float, 2u> speed,
	   Wasp *immune) noexcept
  : position(position)
  , speed(speed)
  , immune(immune)
{
}

Nail::~Nail() noexcept = default;

void Nail::update() noexcept
{
  speed += claws::vect<float, 2u>(float(rand() & 3) - 1.5f, float(rand() & 3) - 1.5f) * 0.0004f;
  speed[1] -= 0.0002f;
  position += speed;
  timer -= !!timer;
}

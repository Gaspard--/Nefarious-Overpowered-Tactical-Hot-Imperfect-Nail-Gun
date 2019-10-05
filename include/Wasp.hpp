#pragma once

#include <claws/container/vect.hpp>
#include <memory>
#include <array>

class Gun;
class Wasp;

namespace state
{
  class GameState;
}

struct WaspSegment
{
  claws::vect<float, 2u> position;
  claws::vect<float, 2u> speed;
  float radius;
  Wasp *wasp;

  void update() noexcept;
};

class Wasp
{
private:
  std::array<uint32_t, 3u> waspSegments;
public:
  float direction;
private:
  uint32_t flyPower{0};
  uint32_t jumpCooldown{0};
  std::unique_ptr<Gun> gun;

public:
  Wasp(state::GameState &gameState, claws::vect<float, 2u> position, float direction, float radius);
  Wasp(Wasp &&wasp) noexcept;
  ~Wasp() noexcept;
  
  std::array<uint32_t, 3u> const &getWaspSegments() const noexcept
  {
    return waspSegments;
  }
  
  uint32_t const &getHead() const noexcept
  {
    return waspSegments[0];
  }

  uint32_t const &getBody() const noexcept
  {
    return waspSegments[1];
  }

  uint32_t const &getAbdommen() const noexcept
  {
    return waspSegments[2];
  }

  void update(state::GameState &) noexcept;
  void fly(state::GameState &) noexcept;
  void fire(state::GameState &gamestate, claws::vect<float, 2u> target);
  void pickUpGun(std::unique_ptr<Gun> &&gun);
};

#pragma once

#include <claws/container/vect.hpp>
#include <memory>
#include <array>
#include <vector>

class Gun;
class Wasp;

namespace state
{
  class GameState;
}

enum class Part
  {
   head,
   body,
   abdomen
  };

struct WaspSegment
{
  claws::vect<float, 2u> position;
  claws::vect<float, 2u> speed;
  float radius;
  Part part;
  Wasp *wasp;
  bool disableCollision{false};

  void update() noexcept;
};

class Wasp
{
private:
  std::vector<uint32_t> victims;
  std::array<uint32_t, 3u> waspSegments;
  bool dead{false};
public:
  bool eating{false};
  float direction;
private:
  uint32_t flyPower{0};
  uint32_t jumpCooldown{0};
public:
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
  void swallow(state::GameState &gameState, uint32_t index);

  bool canBeRemoved() const noexcept
  {
    return dead;
  }
};

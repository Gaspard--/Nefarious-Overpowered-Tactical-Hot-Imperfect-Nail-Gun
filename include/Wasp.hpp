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
  bool unused{false};

  void update() noexcept;

  float getMass() const noexcept
  {
    return std::pow(radius, 3.0f);
  }

  void setMass(float mass) noexcept
  {
    radius = std::pow(mass, 1.0f / 3.0f);
  }
};

claws::vect<float, 2u> springForce(claws::vect<float, 2u> diff, claws::vect<float, 2u> speedDiff, float springSize) noexcept;
void applyForce(WaspSegment &a, WaspSegment &b, claws::vect<float, 2u> force) noexcept;

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
  float inBelly{0.0f};
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
  void removePart(Part segment) noexcept;

  bool canBeRemoved() const noexcept
  {
    return dead;
  }
};

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
  float direction;
  std::unique_ptr<Gun> gun;

  uint32_t flapTimer{0u};
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

  void update(state::GameState &);
};

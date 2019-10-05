#include "Wasp.hpp"
#include "Gun.hpp"
#include "GameState.hpp"

Wasp::~Wasp() noexcept = default;
Wasp::Wasp(Wasp &&wasp) noexcept = default;

Wasp::Wasp(state::GameState &gameState, claws::vect<float, 2u> position, float direction, float radius)
  : waspSegments({
		  gameState.addSegment(WaspSegment{position, claws::vect<float, 2>{0.0f, 0.0f}, radius, this}),
		  gameState.addSegment(WaspSegment{position + claws::vect<float, 2u>{direction * radius * 2.0f, 0.03f},
						   claws::vect<float, 2>{0.0f, 0.0f}, radius, this}),
		  gameState.addSegment(WaspSegment{position + claws::vect<float, 2u>{direction * radius * 2.0f, 0.03f} * 2.0f,
						   claws::vect<float, 2>{0.0f, 0.0f}, radius, this}),
    })
  , direction(direction)
  , gun(nullptr)
{
}


void Wasp::update(state::GameState &gamestate)
{
  for (int i(0); i < 2; ++i)
    {
      WaspSegment &waspSegment0(gamestate.getWaspSegment(waspSegments[i]));
      WaspSegment &waspSegment1(gamestate.getWaspSegment(waspSegments[i + 1]));

      auto diff(waspSegment0.position - waspSegment1.position + claws::vect<float, 2u>{direction * (waspSegment0.radius + waspSegment1.radius), 0.03f});
      auto dir(diff.normalized());
      auto len(std::sqrt(diff.length2()));
      auto speedDiff(dir.scalar(waspSegment0.speed - waspSegment1.speed));
      auto springSize((waspSegment0.radius + waspSegment1.radius) * 0.08f);

      waspSegment0.speed -= dir * ((len - springSize) * 0.1f + speedDiff * 0.07f);
      waspSegment1.speed += dir * ((len - springSize) * 0.1f + speedDiff * 0.07f);
    }
  if ((++flapTimer %= 30) > 15)
    {
      gamestate.getWaspSegment(getBody()).speed[1] += 0.007f;
    }
}

void WaspSegment::update() noexcept
{
  position += speed;
  speed *= 0.99f;
  speed[1] += -0.001f;
}

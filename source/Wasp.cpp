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


void Wasp::update(state::GameState &gameState) noexcept
{
  for (int i(0); i < 2; ++i)
    {
      WaspSegment &waspSegment0(gameState.getWaspSegment(waspSegments[i]));
      WaspSegment &waspSegment1(gameState.getWaspSegment(waspSegments[i + 1]));

      auto diff(waspSegment0.position - waspSegment1.position + claws::vect<float, 2u>{direction * (waspSegment0.radius + waspSegment1.radius), 0.03f});
      auto dir(diff.normalized());
      auto len(std::sqrt(diff.length2()));
      auto speedDiff(dir.scalar(waspSegment0.speed - waspSegment1.speed));
      auto springSize((waspSegment0.radius + waspSegment1.radius) * 0.08f);

      waspSegment0.speed -= dir * ((len - springSize) * 0.1f + speedDiff * 0.07f);
      waspSegment1.speed += dir * ((len - springSize) * 0.1f + speedDiff * 0.07f);
    }
  jumpCooldown -= !!jumpCooldown;
  if (gun)
    {
      gameState.getWaspSegment(getBody()).speed[1] += -0.004f;
      gun->update();
    }
}

void Wasp::fly(state::GameState &gameState) noexcept
{
  if (!jumpCooldown)
    flyPower = 15;
  if (!!flyPower)
    {
      jumpCooldown = 15;
      --flyPower;
      gameState.getWaspSegment(getBody()).speed[1] += 0.01f;
    }
}

void Wasp::fire(state::GameState &gameState, claws::vect<float, 2u> target)
{
  if (gun)
    {
      auto dir((target - gameState.getWaspSegment(getBody()).position).normalized());

      gun->fire(gameState, this, gameState.getWaspSegment(getBody()).position, dir);
    }
}

void Wasp::pickUpGun(std::unique_ptr<Gun> &&gun)
{
  this->gun.swap(gun);
}

void WaspSegment::update() noexcept
{
  position += speed;
  speed *= 0.96f;
  speed[1] += -0.0005f;
}

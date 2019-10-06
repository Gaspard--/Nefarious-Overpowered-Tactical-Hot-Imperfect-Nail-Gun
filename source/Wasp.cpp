#include "Wasp.hpp"
#include "Gun.hpp"
#include "GameState.hpp"

Wasp::~Wasp() noexcept = default;

Wasp::Wasp(Wasp &&wasp) noexcept = default;

Wasp::Wasp(state::GameState &gameState, claws::vect<float, 2u> position, float direction, float radius)
  : waspSegments({
		  gameState.addSegment(WaspSegment{position, claws::vect<float, 2>{0.0f, 0.0f}, radius, Part::head, this}),
		  gameState.addSegment(WaspSegment{position - claws::vect<float, 2u>{direction * radius * 2.0f, 0.03f},
						   claws::vect<float, 2>{0.0f, 0.0f}, radius, Part::body, this}),
		  gameState.addSegment(WaspSegment{position - claws::vect<float, 2u>{direction * radius * 2.0f, 0.03f} * 2.0f,
						   claws::vect<float, 2>{0.0f, 0.0f}, radius, Part::abdomen, this}),
    })
  , direction(direction)
  , gun(nullptr)
{
}


void Wasp::update(state::GameState &gameState) noexcept
{
  if (dead)
    return;
  
  for (int i(0); i < 2; ++i)
    {
      if (!~waspSegments[i * 2] || (!gameState.getWaspSegment(waspSegments[i * 2]).radius && !gameState.getWaspSegment(waspSegments[i * 2]).radius)) // relevant end segment detached, or both parts eaten
	continue;
      WaspSegment &waspSegment0(gameState.getWaspSegment(waspSegments[i]));
      WaspSegment &waspSegment1(gameState.getWaspSegment(waspSegments[i + 1]));

      auto diff(waspSegment0.position - waspSegment1.position - claws::vect<float, 2u>{direction * (waspSegment0.radius + waspSegment1.radius), 0.03f});
      auto dir(diff.normalized());
      auto len(std::sqrt(diff.length2()));
      auto speedDiff(dir.scalar(waspSegment0.speed - waspSegment1.speed));
      auto springSize((waspSegment0.radius + waspSegment1.radius) * 0.08f);
      auto strength(((len - springSize) * 0.07f + speedDiff * 0.1f));

      if (strength >= waspSegment0.radius + waspSegment1.radius)
	{
	  waspSegment0.speed += dir * strength * 4.0f;
	  waspSegment1.speed -= dir * strength * 4.0f;
	  gameState.getWaspSegment(waspSegments[i * 2]).wasp = nullptr;
	  waspSegments[i * 2] = ~0u;
	}
      else
	{
	  waspSegment0.speed -= dir * strength;
	  waspSegment1.speed += dir * strength;
	}
    }
  if (eating && !~getHead())
    {
      for (auto &victim : victims)
	gameState.getWaspSegment(victim).position = gameState.getWaspSegment(victim).position * 0.9f + gameState.getWaspSegment(getHead()).position * 0.1f;
    }
  else
    {
      // TODO: mark as unused
      // for (auto &victim : victims)
      //   gameState.getWaspSegment(victim).unused = true;
      victims.clear();
    }

  jumpCooldown -= !!jumpCooldown;
  if (gun)
    {
      gameState.getWaspSegment(getBody()).speed[1] += -0.004f;
      gun->update();
    }
  dead = !~waspSegments[0] && !~waspSegments[2];
  if (dead)
    for (auto &waspSegment : waspSegments)
      if (~waspSegment)
	gameState.getWaspSegment(waspSegment).wasp = nullptr;
}

void Wasp::fly(state::GameState &gameState) noexcept
{
  if (dead)
    return;
  if (!jumpCooldown)
    flyPower = 15;
  if (!!flyPower)
    {
      jumpCooldown = 30;
      --flyPower;
      gameState.getWaspSegment(getBody()).speed[1] += 0.015f;
    }
}

void Wasp::fire(state::GameState &gameState, claws::vect<float, 2u> target)
{
  if (dead)
    return;
  if (gun)
    {
      auto dir((target - gameState.getWaspSegment(getBody()).position).normalized());

      gun->fire(gameState, this, gameState.getWaspSegment(getBody()).position, dir);
    }
}

void Wasp::pickUpGun(std::unique_ptr<Gun> &&gun)
{
  if (dead)
    return;
  this->gun.swap(gun);
}

void Wasp::swallow(state::GameState &gameState, uint32_t index)
{
  if (dead)
    return;
  gameState.getWaspSegment(getHead()).radius = std::pow(std::pow(gameState.getWaspSegment(getHead()).radius, 3.0f)
							+ std::pow(gameState.getWaspSegment(index).radius, 3.0f), 1.0f / 3.0f);
  if (gameState.getWaspSegment(index).wasp)
    victims.emplace_back(index);
  // TODO: mark as unused
  //  else
  //    gameState.getWaspSegment(index).unused = true;
  gameState.getWaspSegment(index).disableCollision = true;
  gameState.getWaspSegment(index).radius = 0.0f;
}


void WaspSegment::update() noexcept
{
  if (speed.length2() > radius * radius * 0.5f * 0.5f)
    speed = speed.normalized() * radius * 0.5f;
  position += speed;
  speed *= 0.95f;
  speed[1] += -0.0005f;
}

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

// void doSprint(WaspSegment &a, WaspSegment &b, claws::vect<float, 2u> diff, float springSize) noexcept
// {
//   WaspSegment &waspSegment0(gameState.getWaspSegment(waspSegments[i]));
//   WaspSegment &waspSegment1(gameState.getWaspSegment(waspSegments[i + 1]));

//   auto diff(waspSegment0.position - waspSegment1.position - claws::vect<float, 2u>{direction * (waspSegment0.radius + waspSegment1.radius), 0.03f});
//   auto dir(diff.normalized());
//   auto len(std::sqrt(diff.length2()));
//   auto speedDiff(dir.scalar(waspSegment0.speed - waspSegment1.speed));
//   auto springSize((waspSegment0.radius + waspSegment1.radius) * 0.08f);
//   auto strength(((len - springSize) * 0.07f + speedDiff * 0.3f));

//   // if (strength >= waspSegment0.radius + waspSegment1.radius)
//   // 	{
//   // 	  waspSegment0.speed += dir * strength * 9.0f;
//   // 	  waspSegment1.speed -= dir * strength * 9.0f;
//   // 	  gameState.getWaspSegment(waspSegments[i * 2]).wasp = nullptr;
//   // 	  waspSegments[i * 2] = ~0u;
//   // 	}
//   // else
//   {
//     waspSegment0.speed -= dir * strength;
//     waspSegment1.speed += dir * strength;
//   }
// }

claws::vect<float, 2u> springForce(claws::vect<float, 2u> diff, claws::vect<float, 2u> speedDiff, float springSize) noexcept
{
  auto len(std::sqrt(diff.length2()));
  auto dir(diff.normalized());
  auto speedDiffProj(dir.scalar(speedDiff));

  return dir * ((len - springSize) * 0.07f + speedDiffProj * 0.3f);
}

void applyForce(WaspSegment &a, WaspSegment &b, claws::vect<float, 2u> force) noexcept
{
  auto massSum = a.getMass() + b.getMass();

  a.speed -= force * b.getMass() / massSum;
  b.speed += force * a.getMass() / massSum;
}

void Wasp::update(state::GameState &gameState) noexcept
{
  if (dead)
    return;
  {
    constexpr std::array<float, 3u> const ratio{{0.25f, 0.15f, 0.6f}};
    std::array<float, 3u> mass;

    inBelly *= 0.995f;
    float total(0.0f);

    for (int i(0); i < 3; ++i)
      {
	if (~waspSegments[i])
	  mass[i] = gameState.getWaspSegment(waspSegments[i]).getMass();
	else
	  mass[i] = 0.0f;
	total += mass[i];
      }
    total -= inBelly;
    for (int i(0); i < 2; ++i)
      {
	if (mass[i] && mass[i + 1])
	  {
	    mass[i + 1] -= (total * ratio[i] - mass[i]) * 0.04f;
	    mass[i] += (total * ratio[i] - mass[i]) * 0.04f;
	  }
      }
    mass[2] = total - mass[1] - mass[0] + inBelly;
    for (int i(0); i < 3; ++i)
      {
	if (~waspSegments[i])
	  gameState.getWaspSegment(waspSegments[i]).setMass(mass[i]);
      }
  }

  for (int i(0); i < 2; ++i)
    {
      if (!~waspSegments[i * 2] || (!gameState.getWaspSegment(waspSegments[i * 2]).radius && !gameState.getWaspSegment(waspSegments[i * 2]).radius)) // relevant end segment detached, or both parts eaten
	continue;
      {
	WaspSegment &waspSegment0(gameState.getWaspSegment(waspSegments[i]));
	WaspSegment &waspSegment1(gameState.getWaspSegment(waspSegments[i + 1]));

	auto diff(waspSegment0.position - waspSegment1.position - claws::vect<float, 2u>{direction * (waspSegment0.radius + waspSegment1.radius), 0.03f});
	auto springSize((waspSegment0.radius + waspSegment1.radius) * 0.08f);
	auto force(springForce(diff, waspSegment0.speed - waspSegment1.speed, springSize));

	// if (strength >= waspSegment0.radius + waspSegment1.radius)
	// 	{
	// 	  waspSegment0.speed += dir * strength * 9.0f;
	// 	  waspSegment1.speed -= dir * strength * 9.0f;
	// 	  gameState.getWaspSegment(waspSegments[i * 2]).wasp = nullptr;
	// 	  waspSegments[i * 2] = ~0u;
	// 	}
	// else
	applyForce(waspSegment0, waspSegment1, force);
      }
      {
	WaspSegment &waspSegment0(gameState.getWaspSegment(waspSegments[i]));
	WaspSegment &waspSegment1(gameState.getWaspSegment(waspSegments[i + 1]));

	auto diff(waspSegment0.position - waspSegment1.position);
	auto springSize(waspSegment0.radius + waspSegment1.radius);
	auto force(springForce(diff, waspSegment0.speed - waspSegment1.speed, springSize));

	// if (strength >= waspSegment0.radius + waspSegment1.radius)
	// 	{
	// 	  waspSegment0.speed += dir * strength * 9.0f;
	// 	  waspSegment1.speed -= dir * strength * 9.0f;
	// 	  gameState.getWaspSegment(waspSegments[i * 2]).wasp = nullptr;
	// 	  waspSegments[i * 2] = ~0u;
	// 	}
	// else
	applyForce(waspSegment0, waspSegment1, force);
      }
    }
  if (eating && ~getHead())
    {
      for (auto &victim : victims)
	{
	  auto &victimPart(gameState.getWaspSegment(victim));
	  auto &head(gameState.getWaspSegment(getHead()));
	  auto force(springForce(head.position - victimPart.position, head.speed - victimPart.speed, 0.0f));

	  applyForce(head, victimPart, force);

	  float mass = victimPart.getMass();

	  float eaten = mass * 0.1f;
	  if (victimPart.radius < head.radius * 0.1f) {
	    eaten = mass;
	    gameState.removeWaspSegment(victim);
	  }
	  victimPart.setMass(mass - eaten);
	  head.setMass(head.getMass() + eaten);
	  inBelly += eaten;
  	}
    }
  else
    {
      for (auto &victim : victims)
	{
	  gameState.getWaspSegment(victim).disableCollision = false;
	}
      victims.clear();
    }

  jumpCooldown -= !!jumpCooldown;
  if (gun)
    {
      gameState.getWaspSegment(getBody()).speed[1] += -0.004f;
      gun->update();
    }
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

      if (!gun->fire(gameState, this, gameState.getWaspSegment(getBody()).position, dir)) {
	gun->position = gameState.getWaspSegment(getBody()).position;
	gun->speed = {0.0f, 0.01f};
	gameState.looseGun(std::move(gun));
      }
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
  // gameState.getWaspSegment(getHead()).radius = std::pow(std::pow(gameState.getWaspSegment(getHead()).radius, 3.0f)
  // 							+ std::pow(gameState.getWaspSegment(index).radius, 3.0f), 1.0f / 3.0f);
  // if (gameState.getWaspSegment(index).wasp)
  victims.emplace_back(index);
  // TODO: mark as unused
  //  else
  //    gameState.getWaspSegment(index).unused = true;
  gameState.getWaspSegment(index).disableCollision = true;
  // gameState.getWaspSegment(index).radius = 0.0f;
}

void Wasp::removePart(Part segment) noexcept
{
  waspSegments[size_t(segment)] = ~0u;
  dead = (!~waspSegments[0] && !~waspSegments[2]) || !~waspSegments[1];
}

void WaspSegment::update() noexcept
{
  if (speed.length2() > radius * radius * 0.5f * 0.5f)
    speed = speed.normalized() * radius * 0.5f;
  position += speed;
  speed *= 0.95f;
  speed[1] += -0.0005f;
}

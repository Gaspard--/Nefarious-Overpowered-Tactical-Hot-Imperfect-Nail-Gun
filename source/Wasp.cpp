#include "Wasp.hpp"
#include "Gun.hpp"
#include "GameState.hpp"

Wasp::~Wasp() noexcept = default;

Wasp::Wasp(Wasp &&wasp) noexcept = default;

Wasp::Wasp(state::GameState &gameState, claws::vect<float, 2u> position, float direction, float radius)
  : waspSegments({
		  gameState.addSegment(WaspSegment{position, claws::vect<float, 2>{0.0f, 0.0f}, radius, Part::head, this}),
		  gameState.addSegment(WaspSegment{position - claws::vect<float, 2u>{direction * radius, 0.03f},
						   claws::vect<float, 2>{0.0f, 0.0f}, radius, Part::body, this}),
		  gameState.addSegment(WaspSegment{position - claws::vect<float, 2u>{direction * radius, 0.03f} * 2.0f,
						   claws::vect<float, 2>{0.0f, 0.0f}, radius, Part::abdomen, this}),
    })
  , direction(direction)
  , gun(nullptr)
{
}

claws::vect<float, 2u> springForce(claws::vect<float, 2u> diff, claws::vect<float, 2u> speedDiff, float springSize) noexcept
{
  auto len(std::sqrt(diff.length2()));
  auto dir(diff.normalized());
  auto speedDiffProj(dir.scalar(speedDiff));

  return dir * ((len - springSize) * 0.14f + speedDiffProj * 0.3f);
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
  flyFrame = (flyFrame + 1) % 20;
  {
    constexpr std::array<float, 3u> const ratio{{0.25f, 0.15f, 0.6f}};
    std::array<float, 3u> mass;

    inBelly *= 0.997f;
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
    if (total < 0.0f)
      {
	die(gameState);
        return ;
      }
    for (int i(0); i < 2; ++i)
      {
	if (mass[i] && mass[i + 1])
	  {
	    auto delta((total * ratio[i] - mass[i]) * 0.04f);

	    mass[i + 1] -= delta;
	    mass[i] += delta;
	  }
      }
    mass[2] = total - mass[1] - mass[0] + inBelly;
    for (int i(0); i < 3; ++i)
      {
	if (~waspSegments[i])
	  {
	    if (mass[i] < 0.0f)
	      {
		die(gameState);
		return ;
	      }
	    gameState.getWaspSegment(waspSegments[i]).setMass(mass[i]);
	  }
      }
  }

  for (int i(0); i < 2; ++i)
    {
      if (!~waspSegments[i * 2] || (!gameState.getWaspSegment(waspSegments[i * 2]).radius && !gameState.getWaspSegment(waspSegments[i * 2]).radius)) // relevant end segment detached, or both parts eaten
	continue;
      {
	WaspSegment &waspSegment0(gameState.getWaspSegment(waspSegments[i]));
	WaspSegment &waspSegment1(gameState.getWaspSegment(waspSegments[i + 1]));

	auto diff(waspSegment0.position - waspSegment1.position - claws::vect<float, 2u>{direction * (waspSegment0.radius + waspSegment1.radius), 0.0f} * 0.9f);
	auto springSize((waspSegment0.radius + waspSegment1.radius) * 0.08f);
	auto force(springForce(diff, waspSegment0.speed - waspSegment1.speed, springSize));

	applyForce(waspSegment0, waspSegment1, force);
      }
      {
	WaspSegment &waspSegment0(gameState.getWaspSegment(waspSegments[i]));
	WaspSegment &waspSegment1(gameState.getWaspSegment(waspSegments[i + 1]));

	auto diff(waspSegment0.position - waspSegment1.position);
	auto springSize((waspSegment0.radius + waspSegment1.radius) * 0.9f);
	auto force(springForce(diff, waspSegment0.speed - waspSegment1.speed, springSize));

	if (diff.length2() >= (waspSegment0.radius + waspSegment1.radius) * (waspSegment0.radius + waspSegment1.radius) * 16.0f)
	  {
	    applyForce(waspSegment0, waspSegment1, -force * 5.0f);
	    die(gameState);
	    return ;
	  }
	else
	  applyForce(waspSegment0, waspSegment1, force);
      }
    }
  if (eating && ~getHead() && ~getBody() && ~getAbdommen())
    {
      for (auto &victim : victims)
	{
	  auto &victimPart(gameState.getWaspSegment(victim));
	  auto &head(gameState.getWaspSegment(getHead()));
	  auto force(springForce(head.position - victimPart.position, head.speed - victimPart.speed, 0.0f));

	  if (!(rand() & 255))
	    gameState.spawnBlood(victimPart.position, (claws::vect<float, 2u>((float(rand() & 31) - 15.5f) * 0.012f, 0.1f)) * 0.01f);
	  applyForce(head, victimPart, force * 0.2f);

	  float mass = victimPart.getMass();

	  float eaten = mass * 0.1f;
	  if (victimPart.radius < 0.001f || victimPart.radius < head.radius * 0.5f) {
	    eaten = mass;
	    for (int poi = 0; poi < 8; ++poi)
	      gameState.spawnBlood(victimPart.position, (claws::vect<float, 2u>((float(rand() & 31) - 15.5f) * 0.03f, (float(rand() & 31)) * 0.03f)) * 0.03f);
	    gameState.removeWaspSegment(victim);
	  // } else if (victimPart.radius < head.radius) {
	  //   eaten = mass * 0.1f;
	  } else {
	    eaten = head.getMass() * 0.0001f;
	  }

	  victimPart.setMass(mass - eaten);
	  head.setMass(head.getMass() + eaten * 0.9f);
	  inBelly += eaten;
  	}
      victims.erase(std::remove_if(victims.begin(), victims.end(), [&](auto &victim)
								   {
								     return gameState.getWaspSegment(victim).unused;
								   }), victims.end());
    }
  else
    {
      victims.clear();
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
  if (dead)
    return;
  if (!jumpCooldown)
    flyPower = 30;
  if (!!flyPower)
    {
      jumpCooldown = 30;
      --flyPower;
      gameState.getWaspSegment(getBody()).speed[1] += 0.011f;
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
  victims.emplace_back(index);
  //gameState.getWaspSegment(index).disableCollision = true;
}

void Wasp::die(state::GameState &gameState) noexcept
{
  if (dead)
    return ;

  dead = true;
  for (auto &waspSegment : waspSegments)
    if (~waspSegment)
      {
	auto segment(gameState.getWaspSegment(waspSegment));

	removePart(gameState, segment.part);
      }
}

void Wasp::removePart(state::GameState &gameState, Part segment) noexcept
{
  if (segment == Part::body && gun)
    {
      gun->position = gameState.getWaspSegment(getBody()).position;
      gun->speed = {0.0f, 0.01f};
      gameState.looseGun(std::move(gun));
    }

  gameState.getWaspSegment(waspSegments[size_t(segment)]).wasp = nullptr;
  waspSegments[size_t(segment)] = ~0u;
  for (auto &victim : victims)
    {
      //gameState.getWaspSegment(victim).disableCollision = false;
    }
  victims.clear();

  if ((!~waspSegments[0] && !~waspSegments[2]) || !~waspSegments[1])
    die(gameState);
}

void WaspSegment::update() noexcept
{
  if (speed.length2() > radius * radius * 0.5f * 0.5f)
    speed = speed.normalized() * radius * 0.5f;
  position += speed;
  speed *= 0.95f;
  speed[1] += -0.0003f;
  if (part == Part::body)
    speed[1] += -0.0003f;
}

#include "GameState.hpp"
#include "SoundHandler.hpp"
#include "Wasp.hpp"
#include "Nail.hpp"
#include "Gun.hpp"

#include <iostream>
#include <algorithm>
#include <cassert>

struct WaspSegmentNailer
{
  claws::vect<float, 2u> position;
  uint32_t waspSegmentId;
};

struct WaspToWaspNailer
{
  uint32_t waspSegmentId0;
  uint32_t waspSegmentId1;
};

namespace state
{

  GameState::GameState()
    : map({2.0f, 1.0f})
  {
    wasps.emplace_back(new Wasp(*this,
				claws::vect<float, 2u>{0.9f, 1.5f},
				1.0f,
				0.03f));
    for (float i = 0.0f; i < 5.5f; ++i)
      {
    	wasps.emplace_back(new Wasp(*this,
    				    claws::vect<float, 2u>{0.6f * i + 2.0f, 1.0f},
    				    1.0f,
    				    0.01f * (2.0f + i)));
      }
    wasps.front()->pickUpGun(std::unique_ptr<Gun>(guns::makeNothing()));
    bloodPos.resize(bloodCount);
    bloodSpeed.resize(bloodCount);
  }

  GameState::~GameState() noexcept = default;

  float GameState::getGameSpeed()
  {
    return gameSpeed;
  }

  uint32_t GameState::addSegment(WaspSegment &&waspSegment)
  {
    waspSegments.emplace_back(waspSegment);
    return uint32_t(waspSegments.size() - 1);
  }

  void GameState::ai()
  {
    auto &player(wasps.front());

    for (auto it = wasps.begin() + 1; it != wasps.end(); ++it)
      {
	if ((*it)->canBeRemoved())
	  continue;
	// reset state & ai not linked to player
	(*it)->eating = false;
	if (player->canBeRemoved())
	  continue;
	if (getWaspSegment((*it)->getBody()).position[1] < getWaspSegment(player->getBody()).position[1])
	  (*it)->fly(*this);
	(*it)->direction *= 0.9f;
	if (getWaspSegment((*it)->getBody()).position[0] < getWaspSegment(player->getBody()).position[0])	
	  (*it)->direction += 0.1f;
	else
	  (*it)->direction -= 0.1f;

	getWaspSegment((*it)->getBody()).speed[0] += 0.001f * (*it)->direction;
	  
	(*it)->fire(*this, getWaspSegment(wasps.front()->getBody()).position);
	(*it)->eating = true;
      }
  }

  void GameState::collisionCheck()
  {
    constexpr static float gridSize = 1.0f;

    std::unordered_map<size_t, std::vector<uint32_t>> waspPartIndexMap;
    for (uint32_t i = 0; i < waspSegments.size(); ++i)
      {
	auto &waspSegment(waspSegments[i]);
	if (waspSegment.disableCollision)
	  continue;

	claws::vect<int32_t, 2> min;
	claws::vect<int32_t, 2> max;
	for (int j = 0; j < 2; ++j)
	  {
	    min[j] = int32_t(std::floor((waspSegment.position[j] - waspSegment.radius) / gridSize));
	    max[j] = int32_t(std::floor((waspSegment.position[j] + waspSegment.radius) / gridSize));
	  }
	claws::vect<int32_t, 2> tile;
	for (tile[0] = min[0]; tile[0] <= max[0]; ++tile[0])
	  for (tile[1] = min[1]; tile[1] <= max[1]; ++tile[1])
	    {
	      auto &output(waspPartIndexMap[uint32_t(tile[0]) + (size_t(tile[1]) << 32l)]);

	      for (auto &index : output)
		{
		  auto &otherWaspSegment(waspSegments[index]);
		  if (otherWaspSegment.wasp && otherWaspSegment.wasp == waspSegment.wasp)
		    continue;
		  auto diff(otherWaspSegment.position - waspSegment.position);

		  if ((otherWaspSegment.radius + waspSegment.radius) * (otherWaspSegment.radius + waspSegment.radius) > diff.length2())
		    {
		      bool skipCollision = false;
		      if (waspSegment.wasp && waspSegment.part == Part::head && waspSegment.wasp->eating)
			{
			  waspSegment.wasp->swallow(*this, index);
			  skipCollision = true;
			}
		      if (otherWaspSegment.wasp && otherWaspSegment.part == Part::head && otherWaspSegment.wasp->eating)
			{
			  otherWaspSegment.wasp->swallow(*this, i);
			  skipCollision = true;
			}

		      if (!skipCollision && (otherWaspSegment.speed - waspSegment.speed).scalar(diff) < 0)
			{
			  auto dir(diff.normalized());
			  auto rebound(dir * 2.0f * (otherWaspSegment.speed - waspSegment.speed).scalar(dir));
			  auto waspSegmentWeight(std::pow(waspSegment.radius, 3.0f));
			  auto otherWaspSegmentWeight(std::pow(otherWaspSegment.radius, 3.0f));

			  waspSegment.speed += rebound * otherWaspSegmentWeight / (waspSegmentWeight + otherWaspSegmentWeight);
			  otherWaspSegment.speed -= rebound * waspSegmentWeight / (waspSegmentWeight + otherWaspSegmentWeight);
			  otherWaspSegment.speed += diff / diff.length2() * 0.0001f * otherWaspSegmentWeight / (waspSegmentWeight + otherWaspSegmentWeight);
			  waspSegment.speed -= diff / diff.length2() * 0.0001f * waspSegmentWeight / (waspSegmentWeight + otherWaspSegmentWeight);
			}
		    }
		}
	      output.emplace_back(i);
	    }
      }
    for (auto &nail : nails)
      {
	claws::vect<int32_t, 2> min;
	claws::vect<int32_t, 2> max;
	for (int j = 0; j < 2; ++j)
	  {
	    min[j] = int32_t(std::floor((nail.position[j] - nail.speed[j]) / gridSize));
	    max[j] = int32_t(std::floor((nail.position[j]) / gridSize));
	    if (min[j] > max[j])
	      std::swap(min[j], max[j]);
	  }
	claws::vect<int32_t, 2> tile;
	for (tile[0] = min[0]; tile[0] <= max[0]; ++tile[0])
	  for (tile[1] = min[1]; tile[1] <= max[1]; ++tile[1])
	    {
	      auto &output(waspPartIndexMap[uint32_t(tile[0]) + (size_t(tile[1]) << 32l)]);

	      for (auto &index : output)
		{
		  auto &waspSegment(waspSegments[index]);
		  if (waspSegment.wasp && waspSegment.wasp == nail.immune)
		    continue;
		  auto diff(waspSegment.position - nail.position);
		  if (std::abs(diff.scalar(claws::vect<float, 2u>{nail.speed[1], -nail.speed[0]}.normalized())) > waspSegment.radius)
		    continue;
		  if (diff.scalar(nail.speed) > 0.0f && diff.length2() > waspSegment.radius * waspSegment.radius)
		    continue;
		  // note: -(-nail.speed) is +nail.speed
		  if ((diff + nail.speed).scalar(nail.speed) < 0.0f && (nail.speed + diff).length2() > waspSegment.radius * waspSegment.radius)
		    continue;

		  if (~nail.waspSegmentStick)
		    {
		      if (waspSegment.wasp && waspSegment.wasp != getWaspSegment(nail.waspSegmentStick).wasp)
			{
			  waspToWaspNailers.emplace_back(WaspToWaspNailer{nail.waspSegmentStick, index});
			  nail.timer = 0;
			}
		    }
		  else
		    {
		      waspSegment.speed += nail.speed;
		      waspSegment.radius *= 0.95f;
		      nail.speed *= 0.8f;
		      nail.timer = std::min(nail.timer, 2u);
		      nail.waspSegmentStick = index;
		      for (int poi = 0; poi < 5; ++poi)
			spawnBlood(nail.position, (nail.speed + claws::vect<float, 2u>((float(rand() & 3) - 1.5f) * 0.05f, 0.06f)) * 0.1f);
		    }
		}
	    }
      }
  }

  void GameState::terrainCheck()
  {
    // do terrain collision
    for (auto &waspSegment : waspSegments)
      {
	map.collision(waspSegment.position, waspSegment.speed, waspSegment.radius * (waspSegment.part == Part::body ? 1.5f : 1.0f),
		      [&waspSegment](claws::vect<float, 2u> collisionPoint)
		      {
			auto diff(collisionPoint - waspSegment.position);
			auto dir(diff.normalized());

			waspSegment.speed -= dir * 1.9f * waspSegment.speed.scalar(dir);
			waspSegment.position = collisionPoint - dir * waspSegment.radius * (waspSegment.part == Part::body ? 1.5f : 1.0f);
		      });
      }
    for (auto &gun : guns)
      {
	map.collision(gun->position, gun->speed, gun->radius,
		      [&gun](claws::vect<float, 2u> collisionPoint)
		      {
			auto diff(collisionPoint - gun->position);
			auto dir(diff.normalized());

			gun->speed -= dir * 1.8f * gun->speed.scalar(dir);
			gun->position = collisionPoint - dir * gun->radius;
		      });
      }
    for (auto &nail : nails)
      map.collision(nail.position, nail.speed, 0.0f, [&](claws::vect<float, 2u> collisionPoint)
						     {
						       nail.position = collisionPoint;
						       nail.timer = 0;
						       if (~nail.waspSegmentStick)
							 {
							   waspSegmentNailers.emplace_back(WaspSegmentNailer{nail.position - nail.speed.normalized() * getWaspSegment(nail.waspSegmentStick).radius, nail.waspSegmentStick});
							 }
						     });
  }


  StateType GameState::update(unsigned int &)
  {
    timer += getGameSpeed();
    SoundHandler::getInstance().setGlobalPitch(getGameSpeed());
    auto &player(wasps.front());

    if (!player->canBeRemoved())
      {
	zoom *= 0.9f;
	zoom += 0.05f / (getWaspSegment(wasps.front()->getBody()).radius + 0.01f) * 0.1f;
      }
    if (!waspSegments.front().unused)
      {
	offset *= 0.9f;
	offset += -getWaspSegment(0).position * 0.1f;
      }

    map.setMapPosition(claws::vect_cast<int>(-getOffset() / tileSize));

    if (screenShake > 0)
      {
	screenShake = std::max(screenShake - getGameSpeed(), 0.0f);
      }

    for (auto &wasp : wasps)
      wasp->update(*this);
    
    for (auto gun = guns.begin() ; gun != guns.end() ; ++gun) {
      (*gun)->update();
      for (auto &wasp : wasps)
	if (!wasp->canBeRemoved() && (*gun)->getHeat() < 0.1f && ((*gun)->position - getWaspSegment(wasp->getBody()).position).length2() < pow((*gun)->radius + getWaspSegment(wasp->getBody()).radius, 2.0)) {
	  wasp->pickUpGun(std::move((*gun)));
	  break;
	}
      if (!(*gun)) {
	gun = guns.erase(gun);
	--gun;
      }
    }
    if (!player->canBeRemoved())
      {
	getWaspSegment(player->getBody()).speed[0] += right * 0.005f;
	if (up)
	  player->fly(*this);
	if (right != 0.0f)
	  (player->direction *= 0.7f) += right * 0.3f;
	if (firing)
	  player->fire(*this, (target / getZoom() - getOffset()));
	player->eating = eating;
      }
    ai();
    for (auto &waspSegment : waspSegments)
      waspSegment.update();
    for (auto &nail : nails)
      nail.update();
    for (auto &waspSegmentNailer : waspSegmentNailers)
      {
	WaspSegment &waspSegment(getWaspSegment(waspSegmentNailer.waspSegmentId));

	waspSegment.speed = {0.0f, 0.0f};
	waspSegment.position = waspSegmentNailer.position;
      }
    for (auto &waspToWaspNailer : waspToWaspNailers)
      {
	WaspSegment &waspSegment0(getWaspSegment(waspToWaspNailer.waspSegmentId0));
	WaspSegment &waspSegment1(getWaspSegment(waspToWaspNailer.waspSegmentId1));


	auto diff(waspSegment0.position - waspSegment1.position);
	auto dir(diff.normalized());
	auto len(std::sqrt(diff.length2()));
	auto speedDiff(dir.scalar(waspSegment0.speed - waspSegment1.speed));
	auto springSize((waspSegment0.radius + waspSegment1.radius) * 0.08f);

	waspSegment0.speed -= dir * ((len - springSize) * 0.1f + speedDiff * 0.1f);
	waspSegment1.speed += dir * ((len - springSize) * 0.1f + speedDiff * 0.1f);
      }

    wasps.erase(std::remove_if(wasps.begin() + 1, wasps.end(),
			       [](auto const &wasp)
			       {
				 return wasp->canBeRemoved();
			       }), wasps.end());

    collisionCheck();
    // remove nails that are gone to avoid "passing through" effect
    nails.erase(std::remove_if(nails.begin(), nails.end(),
			       [](auto const &nail)
			       {
				 return nail.canBeRemoved();
			       }), nails.end());

    terrainCheck();
    // remove nails that are gone to avoid "passing through" effect
    nails.erase(std::remove_if(nails.begin(), nails.end(),
			       [](auto const &nail)
			       {
				 return nail.canBeRemoved();
			       }), nails.end());


    for (size_t i(0u); i != bloodPos.size(); ++i)
      {
	map.collision(bloodPos[i], bloodSpeed[i], 0.0f,
		      [&](claws::vect<float, 2u> collisionPoint)
		      {
			bloodSpeed[i] *= 0.5f;
		      });

	bloodPos[i] += bloodSpeed[i];
	bloodSpeed[i][1] -= 0.001f;
      }

    
    if (false) // dead
      return GAME_OVER_STATE;
    else if (won)
      return WIN_STATE;
    return StateType::CONTINUE;
  }

  void GameState::handleKey(GLFWwindow *, input::Key)
  {
  }

  void GameState::handleMouse(input::Input const &input, GLFWwindow *, input::Mouse mouse)
  {
    target = claws::vect<float, 2>{float(mouse.x), float(mouse.y)} * 2.0f;
    target -= claws::vect_cast<float>(input.getSize());
    target /= float(input.getSize()[1]);
    target[1] *= -1.0f;
  }

  void GameState::handleButton(GLFWwindow *, input::Button button)
  {
    if (button.button == GLFW_MOUSE_BUTTON_RIGHT && button.action == GLFW_PRESS)
      {
	// handle press
      }
  }

  void GameState::checkEvents(input::Input &input)
  {
    // mouse + keyboard input
    right = float((input.isKeyPressed(GLFW_KEY_D) || input.isKeyPressed(GLFW_KEY_RIGHT)) -
		  (input.isKeyPressed(GLFW_KEY_A) || input.isKeyPressed(GLFW_KEY_LEFT)));
    up = (input.isKeyPressed(GLFW_KEY_W) || input.isKeyPressed(GLFW_KEY_UP));
    firing = (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT));
    eating = (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) || input.isKeyPressed(GLFW_KEY_SPACE));
    // END (mouse + keyboard input)

    std::vector<claws::vect<float, 2>> axes = input.getJoystickAxes();
    std::vector<unsigned char> buttons = input.getJoystickButtons();
    if (axes.size() && axes[0].length2() > 0.1) {
      joystickInUse = true;
      gotoTarget = false;
      joystickVect = {axes[0][0], axes[0][1]
#ifndef _WIN32
		      * -1
#endif
      };
    } else {
      joystickInUse = false;
    }
    if (buttons.size()) {
      if (buttons[0] == GLFW_PRESS && !jsButtonWasPressed[0]) {
	jsButtonWasPressed[0] = true;
	// handle press
      }
      for (unsigned i = 0 ; i < buttons.size() && i < jsButtonWasPressed.size() ; ++i)
	if (buttons[i] == GLFW_RELEASE)
	  jsButtonWasPressed[i] = false;
    }


  }

  void GameState::addNail(claws::vect<float, 2u> position, claws::vect<float, 2u> speed, Wasp *wasp)
  {
    nails.emplace_back(Nail{position, speed, wasp});
  }

  void GameState::getObjectsToRender(DisplayData &displayData)
  {
    auto offset(getOffset());
    auto zoom(getZoom());
    auto apply([&](claws::vect<float, 2u> position)
	       {
		 return (position + offset) * zoom;
	       });
    displayData.mapSize = claws::vect_cast<int>(claws::vect<float, 2u>{4.0f, 2.0f} / getZoom() / tileSize) + 4;
    map.fillDisplayData(displayData.mapOffset, displayData.mapSize, displayData.mapData);
    displayData.offset = offset; // for terrain display, entities are pre-scaled
    displayData.zoom = zoom; // for terrain display, entities are pre-scaled
    displayData.timer = timer;
    displayData.screenShake = screenShake;
    displayData.heat = wasps.front()->gun ? wasps.front()->gun->getHeat() : 0.0f;
    for (auto &waspSegment : waspSegments)
      {
	if (waspSegment.unused)
	  continue;
	claws::vect<float, 2u> invert{waspSegment.wasp && waspSegment.wasp->direction < 0 ? 1.0f : -1.0f, 1.0f};
	switch (waspSegment.part)
	  {
	  case Part::head:
	    displayData.anims[size_t(SpriteId::WaspHead)].emplace_back(AnimInfo{apply(waspSegment.position - invert * waspSegment.radius * 2.2f),
										apply(waspSegment.position + invert * waspSegment.radius * 2.2f),
										0});
	    break;
	  case Part::body:
	    displayData.anims[size_t(SpriteId::WaspBody)].emplace_back(AnimInfo{apply(waspSegment.position - invert * waspSegment.radius * 2.2f),
										apply(waspSegment.position + invert * waspSegment.radius * 2.2f),
										0});
	    if (waspSegment.wasp && waspSegment.wasp->gun)
	      {
		displayData.rotatedAnims[size_t(SpriteId::NailGun)].emplace_back(RotatedAnimInfo{{apply(waspSegment.position - invert * waspSegment.wasp->gun->radius - invert * claws::vect<float, 2>{waspSegment.radius, 0.0f}),
												  apply(waspSegment.position + invert * waspSegment.wasp->gun->radius - invert * claws::vect<float, 2>{waspSegment.radius, 0.0f}),
												  0},
												 (target / getZoom() - getOffset()) - waspSegment.position});

	      }
	    break;
	  case Part::abdomen:
	    displayData.anims[size_t(SpriteId::WaspAbdomen)].emplace_back(AnimInfo{apply(waspSegment.position - invert * waspSegment.radius * 2.2f),
										   apply(waspSegment.position + invert * waspSegment.radius * 2.2f),
										   0});
	    break;
	  default:
	    assert(!"Unhandled wasp segment type ( ? ? ? )");
	  }
      }
    claws::vect<float, 2u> invert{std::cos(timer * 0.05f), 1.0f};
    for (auto &gun : guns) {
      displayData.anims[size_t(SpriteId::NailGun)].emplace_back(AnimInfo{apply(gun->position - invert * gun->radius),
									 apply(gun->position + invert * gun->radius),
									  0});
    }
    for (auto &nail : nails)
      displayData.rotatedAnims[size_t(SpriteId::Nail)].emplace_back(RotatedAnimInfo{{apply(nail.position - 0.02f),
										     apply(nail.position + 0.02f),
										     0},
										    -nail.speed});
    
    displayData.bloodPos = bloodPos;
    displayData.bloodSpeed = bloodSpeed;
  }

  claws::vect<float, 2u> GameState::getOffset() const noexcept
  {
    return offset;
  }

  float GameState::getZoom() const noexcept
  {
    return zoom;
  }

  WaspSegment &GameState::getWaspSegment(size_t index) noexcept
  {
    return waspSegments[index];
  }


  WaspSegment const &GameState::getWaspSegment(size_t index) const noexcept
  {
    return waspSegments[index];
  }

  void GameState::removeWaspSegment(size_t index)
  {
    auto &segment(getWaspSegment(index));
    if (Wasp *wasp = segment.wasp)
      wasp->removePart(*this, segment.part);
    segment.disableCollision = true;
    segment.unused = true;
    waspSegmentNailers.erase(std::remove_if(waspSegmentNailers.begin(), waspSegmentNailers.end(),
					    [&](auto &nailer)
					    {
					      return (nailer.waspSegmentId == index);
					    }), waspSegmentNailers.end());
    waspToWaspNailers.erase(std::remove_if(waspToWaspNailers.begin(), waspToWaspNailers.end(),
					    [&](auto &nailer)
					    {
					      return (nailer.waspSegmentId0 == index || nailer.waspSegmentId1 == index);
					    }), waspToWaspNailers.end());
    if (index >= 3) // avoid reusing player segments
      reusableSegments.emplace_back(index);
  }

  void GameState::spawnBlood(claws::vect<float, 2u> position, claws::vect<float, 2u> speed)
  {
    bloodPos[bloodSpawnIndex] = position;
    bloodSpeed[bloodSpawnIndex] = speed;
    ++bloodSpawnIndex %= bloodPos.size();
  }

  void GameState::looseGun(std::unique_ptr<Gun> &&gun)
  {
    guns.push_back(std::move(gun));
  }
}

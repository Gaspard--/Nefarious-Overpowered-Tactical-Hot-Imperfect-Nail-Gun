#include "GameState.hpp"
#include "SoundHandler.hpp"
#include "Wasp.hpp"
#include "Nail.hpp"
#include "Gun.hpp"

#include <iostream>
#include <algorithm>

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
    : map({2.0, 1.0})
  {
    wasps.emplace_back(new Wasp(*this,
				claws::vect<float, 2u>{0.9f, 0.0f},
				1.0f,
				0.03f));
    for (float i = 0.0f; i < 15.5f; ++i)
      {
	wasps.emplace_back(new Wasp(*this,
				    claws::vect<float, 2u>{0.3f * i, 1.0f},
				    1.0f,
				    0.03f));
      }
    wasps.front()->pickUpGun(std::unique_ptr<Gun>(guns::makeNothing()));
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

  StateType GameState::update(unsigned int &)
  {
    SoundHandler::getInstance().setGlobalPitch(getGameSpeed());
    auto &player(wasps.front());

    if (screenShake > 0)
      {
	screenShake = std::max(screenShake - getGameSpeed(), 0.0f);
      }

    for (auto &wasp : wasps)
      wasp->update(*this);
    getWaspSegment(player->getBody()).speed[0] += right * 0.005f;
    if (up)
      player->fly(*this);
    if (right != 0.0f)
      (player->direction *= 0.7f) += right * 0.3f;
    if (firing)
      player->fire(*this, (target / getZoom() - getOffset()));
    player->eating = eating;
    for (auto it = wasps.begin() + 1; it != wasps.end(); ++it)
      {
	if (getWaspSegment((*it)->getBody()).position[1] < getWaspSegment(player->getBody()).position[1])
	  (*it)->fly(*this);
	(*it)->fire(*this, getWaspSegment(wasps.front()->getBody()).position);
      }
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

	waspSegment0.speed -= dir * ((len - springSize) * 0.1f + speedDiff * 0.07f);
	waspSegment1.speed += dir * ((len - springSize) * 0.1f + speedDiff * 0.07f);
      }

    wasps.erase(std::remove_if(wasps.begin() + 1, wasps.end(),
			       [](auto const &wasp)
			       {
				 return wasp->canBeRemoved();
			       }), wasps.end());

    // do collistion
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
			    if (waspSegment.radius > otherWaspSegment.radius)
			      waspSegment.wasp->swallow(*this, index);
			    skipCollision = true;
			  }
			if (otherWaspSegment.wasp && otherWaspSegment.part == Part::head && otherWaspSegment.wasp->eating)
			  {
			    if (otherWaspSegment.radius > waspSegment.radius)
			      otherWaspSegment.wasp->swallow(*this, i);
			    skipCollision = true;
			  }

			if (!skipCollision && (otherWaspSegment.speed - waspSegment.speed).scalar(diff) < 0)
			  {
			    otherWaspSegment.speed += diff / diff.length2() * 0.001f;
			    waspSegment.speed -= diff / diff.length2() * 0.001f;
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
			waspSegment.speed += nail.speed * 0.2f;
			waspSegment.radius *= 0.95f;
			nail.speed *= 0.8f;
			nail.timer = std::min(nail.timer, 2u);
			nail.waspSegmentStick = index;
		      }
		  }
	      }
	}
    }

    // remove nails that are gone to avoid "passing through" effect
    nails.erase(std::remove_if(nails.begin(), nails.end(),
			       [](auto const &nail)
			       {
				 return nail.canBeRemoved();
			       }), nails.end());
    // do terrain collision
    for (auto &waspSegment : waspSegments)
      for (int i = 0; i < 2; ++i)
	if (waspSegment.position[i] - 0.5f < waspSegment.radius) // stupid ground check for the moment
	  {
	    waspSegment.position[i] = +0.5f + waspSegment.radius;
	    waspSegment.speed[i] *= -0.9f;
	  }
    for (auto &nail : nails)
      for (int i = 0; i < 2; ++i)
	if (nail.position[i] - 0.5f < 0.0f) // stupid ground check for the moment
	  {
	    nail.position[i] = +0.5f;
	    nail.timer = 0;
	    if (~nail.waspSegmentStick)
	      {
		claws::vect<float, 2u> offset{0, 0};

		offset[i] += getWaspSegment(nail.waspSegmentStick).radius;
		waspSegmentNailers.emplace_back(WaspSegmentNailer{nail.position, nail.waspSegmentStick});
	      }
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
    map.fillDisplayData(displayData.mapOffset, displayData.mapSize, displayData.mapData);
    displayData.offset = offset; // for terrain display, entities are pre-scaled
    displayData.zoom = zoom; // for terrain display, entities are pre-scaled
    displayData.timer = timer;
    displayData.screenShake = screenShake;
    displayData.heat = wasps.front()->gun->getHeat();
    for (auto &waspSegment : waspSegments)
      displayData.colors.emplace_back(ColorInfo{apply(waspSegment.position - waspSegment.radius),
						apply(waspSegment.position + waspSegment.radius),
						claws::vect<float, 4u>{0.5f, 0.5f, 0.0f, 1.0f}});
    for (auto &nail : nails)
      displayData.rotatedAnims[size_t(SpriteId::Nail)].emplace_back(RotatedAnimInfo{{apply(nail.position - 0.02f),
										     apply(nail.position + 0.02f),
										     0},
										    -nail.speed});
  }

  claws::vect<float, 2u> GameState::getOffset() const noexcept
  {
    return -getWaspSegment(wasps.front()->getHead()).position;
  }

  float GameState::getZoom() const noexcept
  {
    return 0.05f / getWaspSegment(wasps.front()->getBody()).radius;
  }
  


  WaspSegment &GameState::getWaspSegment(size_t index) noexcept
  {
    return waspSegments[index];
  }


  WaspSegment const &GameState::getWaspSegment(size_t index) const noexcept
  {
    return waspSegments[index];
  }
}

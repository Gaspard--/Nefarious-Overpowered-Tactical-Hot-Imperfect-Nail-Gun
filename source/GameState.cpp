#include "GameState.hpp"
#include "SoundHandler.hpp"
#include "Wasp.hpp"

#include <iostream>
#include <algorithm>

namespace state
{
  GameState::GameState()
  {
    wasps.emplace_back(new Wasp(*this,
				claws::vect<float, 2u>{0.3f, 0.0f},
				1.0f,
				0.05f));
    wasps.emplace_back(new Wasp(*this,
				claws::vect<float, 2u>{0.0f, 0.0f},
				0.0f,
				0.05f));
    wasps.emplace_back(new Wasp(*this,
				claws::vect<float, 2u>{-0.3f, 0.0f},
				-1.0f,
				0.05f));
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

    for (auto &wasp : wasps)
      wasp->update(*this);
    for (auto &waspSegment : waspSegments)
      waspSegment.update();

    // do collistion

    // do terrain collision

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
    gotoTarget = (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT));
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

  void GameState::getObjectsToRender(DisplayData &displayData)
  {
    displayData.timer = timer;
    displayData.screenShake = screenShake;
    for (auto &waspSegment : waspSegments)
      displayData.colors.emplace_back(ColorInfo{waspSegment.position - waspSegment.radius,
						waspSegment.position + waspSegment.radius,
						claws::vect<float, 4u>{0.5f, 0.5f, 0.0f, 1.0f}});

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

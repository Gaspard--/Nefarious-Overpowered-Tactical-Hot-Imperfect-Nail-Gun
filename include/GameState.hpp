#pragma once

#include <optional>
#include <vector>
#include <array>
#include <map>

#include "State.hpp"
#include "MapManager.hpp"

class Wasp;
class WaspSegment;
class Nail;
struct WaspSegmentNailer;
struct WaspToWaspNailer;

namespace state
{
  class GameState : public State
  {
    claws::vect<float, 2u> target;
    bool gotoTarget{false};
    bool joystickInUse{false};
    claws::vect<float, 2u> joystickVect;
    std::array<bool, 1> jsButtonWasPressed;
    float gameSpeed{1.0f};
    float timer{0.0f};
  public:
    float screenShake{0.0f};
  private:
    bool won{false};

    bool firing{false};
    bool eating{false};
    bool up{false};
    float right{0.0f};
    MapManager map;

    std::vector<WaspSegment> waspSegments;
    std::vector<Nail> nails;
    std::vector<WaspSegmentNailer> waspSegmentNailers;
    std::vector<WaspToWaspNailer> waspToWaspNailers;
    std::vector<std::unique_ptr<Wasp>> wasps;
  public:
    GameState();
    ~GameState() noexcept;
    StateType update(unsigned int &time) override;
    void handleKey(GLFWwindow *window, input::Key key) override;
    void handleMouse(input::Input const &, GLFWwindow *window, input::Mouse mouse) override;
    void handleButton(GLFWwindow *window, input::Button button) override;
    void checkEvents(input::Input& input) override;
    void getObjectsToRender(DisplayData &display) override;

    float getGameSpeed();

    uint32_t addSegment(WaspSegment &&waspSegment);

    WaspSegment &getWaspSegment(size_t index) noexcept;

    WaspSegment const &getWaspSegment(size_t index) const noexcept;

    void addNail(claws::vect<float, 2u> position, claws::vect<float, 2u> speed, Wasp *wasp);
  };
}

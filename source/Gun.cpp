#include "Gun.hpp"
#include "GameState.hpp"

namespace guns
{
  Gun *makeNothing()
  {
    class Nothing : public Gun
    {
      int16_t heat{0};
      uint8_t counter{0};
      bool toHot{false};
      bool wasUsed{false};
    public:

      Nothing() {
	radius = 0.05f;
      }

      virtual bool fire(state::GameState &gameState, Wasp *wasp, claws::vect<float, 2u> position, claws::vect<float, 2u> dir) override final
      {
	if (toHot)
	  return false;
	wasUsed = true;
	if (!(++counter %= uint8_t(1 + 10 / (heat / 24 + 1))))
	  {
	    heat = int16_t(heat + (10 / (heat / 24 + 1) + 2));
	    if (heat >= 380)
	      toHot = true;
	    dir += claws::vect<float, 2u>(float(rand() & 3) - 1.5f, float(rand() & 3) - 1.5f) * 0.01f;
	    gameState.addNail(position, dir * 0.06f, wasp);
	    gameState.screenShake = std::max(0.03f, gameState.screenShake);
	  }
	return true;
      }

      virtual void update() override final
      {
	toHot &= heat > 0;
	if (!wasUsed)
	  heat = int16_t(heat - !!heat + !toHot * 2);
	if (heat < 0)
	  heat = 0;
	position += speed;
	speed[1] -= 0.001f;
	wasUsed = false;
      }

      virtual float getHeat() override final
      {
	return float(heat) / 380.0f;
      }

    };
    return new Nothing();
  }
}

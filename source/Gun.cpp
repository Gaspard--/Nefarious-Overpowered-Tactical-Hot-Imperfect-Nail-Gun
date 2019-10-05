#include "Gun.hpp"
#include "GameState.hpp"

namespace guns
{
  Gun *makeNothing()
  {
    class Nothing : public Gun
    {
      uint8_t heat{0};
      uint8_t counter{0};
      bool toHot{false};
    public:
      virtual void fire(state::GameState &gameState, Wasp *wasp, claws::vect<float, 2u> position, claws::vect<float, 2u> dir) override final
      {
	if (toHot)
	  return;
	if (!(++counter %= (1 + 10 / (heat / 24 + 1))))
	  {
	    heat += 10 / (heat / 24 + 1) + 2;
	    if (heat >= 240)
	      toHot = true;
	    dir += claws::vect<float, 2u>(float(rand() & 3) - 1.5f, float(rand() & 3) - 1.5f) * 0.03f;
	    gameState.addNail(position + dir * (float(rand() & 7) * 0.1f) * 0.06f, dir * 0.06f, wasp);
	  }
      }

      virtual void update() override final
      {
	toHot &= heat > 24;
	heat -= !!heat + toHot;
      }

      virtual float getHeat() override final
      {
	return float(heat) / 240.0f;
      }      
    };
    return new Nothing();
  } 
}

#include "Gun.hpp"
#include "GameState.hpp"

namespace guns
{
  Gun *makeNothing()
  {
    class Nothing : public Gun
    {
      uint8_t heat;
      uint8_t counter;
    public:
      virtual void fire(state::GameState &gameState, Wasp *wasp, claws::vect<float, 2u> position, claws::vect<float, 2u> dir) override final
      {
	if (!(++counter %= (16 - heat)))
	  {
	    if (heat < 15)
	      ++heat;
	    dir += claws::vect<float, 2u>(float(rand() & 3) - 1.5f, float(rand() & 3) - 1.5f) * 0.03f;
	    gameState.addNail(position + dir * (float(rand() & 7) * 0.1f) * 0.06f, dir * 0.06f, wasp);
	  }
      }
    };
    return new Nothing();
  } 
}

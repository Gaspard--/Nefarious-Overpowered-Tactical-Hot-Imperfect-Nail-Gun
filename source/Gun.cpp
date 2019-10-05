#include "Gun.hpp"
#include "GameState.hpp"

namespace guns
{
  Gun *makeNothing()
  {
    class Nothing : public Gun
    {
    public:
      virtual void fire(state::GameState &gameState, Wasp *wasp, claws::vect<float, 2u> position, claws::vect<float, 2u> dir) override final
      {
	dir += claws::vect<float, 2u>(float(rand() & 3) - 1.5f, float(rand() & 3) - 1.5f) * 0.03f;
	gameState.addNail(position, dir * 0.06f, wasp);
      }
    };
    return new Nothing();
  } 
}

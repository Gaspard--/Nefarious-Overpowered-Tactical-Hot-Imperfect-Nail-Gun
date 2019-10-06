#pragma once

# include <vector>
# include <string>
# include <algorithm>
# include "claws/container/vect.hpp"
# include "TileId.hpp"

class MapManager
{
  std::vector<std::vector<TileId>> mapTiles;
  claws::vect<int, 2> position{0, 0};
  claws::vect<float, 2> winSize;

  void generateChunk(uint32_t leftExp, uint32_t rightExp, uint32_t upExp, uint32_t downExp);
  void initTestMap();

public:

  MapManager(claws::vect<float, 2> const &winSize);
  void moveMap(claws::vect<int, 2> const &movementSize);

  void fillDisplayData(claws::vect<int, 2> &mapOffset, claws::vect<int, 2> &mapSize, std::vector<TileId> &drawMap) const;

  template<class Func>
  void collision(claws::vect<float, 2> pos, claws::vect<float, 2> speed, float radius, Func &&func)
  {
    claws::vect<float, 2> baseCornerHitboxF = {(pos[0] - radius + winSize[0] / 2.0) / tileSize, (pos[1] - radius + winSize[1] / 2.0) / tileSize};
    claws::vect<unsigned, 2> baseCornerHitbox;

    if (baseCornerHitboxF[0] < 0)
      baseCornerHitbox[0] = 0;
    else
      baseCornerHitbox[0] = (unsigned)baseCornerHitboxF[0];

    if (baseCornerHitboxF[1] < 0)
      baseCornerHitbox[1] = 0;
    else
      baseCornerHitbox[1] = (unsigned)baseCornerHitboxF[1];
    for (unsigned i = baseCornerHitbox[0] ; i * tileSize - winSize[0] / 2.0 < pos[0] + radius ; ++i)
      for (unsigned j = baseCornerHitbox[1] ; j * tileSize - winSize[1] / 2.0 < pos[1] + radius ; ++j) {
	claws::vect<float, 2> collisionPoint = {
						std::max(i * tileSize - winSize[0] / 2.0f, std::min(i * tileSize - winSize[0] / 2.0f + tileSize, pos[0])),
						std::max(j * tileSize - winSize[1] / 2.0f, std::min(j * tileSize - winSize[1] / 2.0f + tileSize, pos[1])),
	};
	switch (mapTiles[position[0] + i][position[1] + j]) {
	case TileId::Wall:
	case TileId::Ground:
	case TileId::Ceil:
	  break;
	case TileId::UpClosedWall:
	  if (speed[1] < 0)
	    break;
	case TileId::DownClosedWall:
	  if (speed[1] > 0)
	    break;
	case TileId::LeftClosedWall:
	  if (speed[0] > 0)
	    break;
	case TileId::RightClosedWall:
	  if (speed[0] < 0)
	    break;
	case TileId::Empty:
	default:
	  continue;
	}
	func(collisionPoint);
      }
  }

};

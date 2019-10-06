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
  void setMapPosition(claws::vect<int, 2> const &position);

  void fillDisplayData(claws::vect<int, 2> &mapOffset, claws::vect<int, 2> &mapSize, std::vector<TileId> &drawMap) const;

  template<class Func>
  void collision(claws::vect<float, 2> pos, claws::vect<float, 2> speed, float radius, Func &&func)
  {
    claws::vect<float, 2> baseCornerHitboxF = (pos - radius) / tileSize;
    claws::vect<unsigned, 2> baseCornerHitbox;

    baseCornerHitbox[0] = unsigned(std::max(baseCornerHitboxF[0], 0.0f));
    baseCornerHitbox[1] = unsigned(std::max(baseCornerHitboxF[1], 0.0f));

    for (unsigned i = baseCornerHitbox[0] ; i * tileSize < pos[0] + radius ; ++i)
      for (unsigned j = baseCornerHitbox[1] ; j * tileSize < pos[1] + radius ; ++j) {
	claws::vect<float, 2> collisionPoint = {
						std::max(i * tileSize, std::min(i * tileSize + tileSize, pos[0])),
						std::max(j * tileSize, std::min(j * tileSize + tileSize, pos[1])),
	};
	switch (mapTiles[i][j]) {
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

#pragma once

# include <vector>
# include <string>
# include <algorithm>
# include <optional>
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

  void fillDisplayData(claws::vect<int, 2> &mapOffset, claws::vect<int, 2> const &mapSize, std::vector<TileId> &drawMap) const;

  TileId getTile(claws::vect<float, 2u> pos) const noexcept;

  template<class Func>
  void collision(claws::vect<float, 2> pos, claws::vect<float, 2> speed, float radius, Func &&func)
  {
    claws::vect<unsigned, 2> center;

    for (int i = 0; i < 2; ++i)
      {
	center[i] = unsigned(std::max(pos[i] / tileSize, 0.0f));
      }

    bool checkCorners = true;
    for (int i = 0; i < 2; ++i)
      {
	claws::vect<unsigned, 2u> tilePos;

	tilePos[!i] = center[!i];
	if (speed[i] > 0)
	  for (unsigned j = center[i]; float(j) * tileSize < pos[i] + radius; ++j)
	    {
	      tilePos[i] = j;
	      if (tile::isFullySolid(getTile(tilePos)) || getTile(tilePos) == (i ? TileId::DownClosedWall : TileId::LeftClosedWall))
		{
		  claws::vect<float, 2> collisionPoint;

		  collisionPoint[i] = float(tilePos[i]) * tileSize;
		  collisionPoint[!i] = pos[!i];

		  func(collisionPoint);
		  checkCorners = false;
		}
	    }
	else
	  for (unsigned j = center[i]; float(int(j)) * tileSize >= std::max(pos[i] - radius, 0.0f); --j)
	    {
	      tilePos[i] = j - 1;
	      if (tile::isFullySolid(getTile(tilePos)) || getTile(tilePos) == (i ? TileId::UpClosedWall : TileId::RightClosedWall))
		{
		  claws::vect<float, 2> collisionPoint;

		  collisionPoint[i] = float(tilePos[i] + 1) * tileSize;
		  collisionPoint[!i] = pos[!i];

		  func(collisionPoint);
		  checkCorners = false;
		}
	    }
      }

    if (!checkCorners)
      return ;
    claws::vect<float, 2> baseCornerHitboxF = (pos - radius) / tileSize;
    claws::vect<unsigned, 2> baseCornerHitbox;
    std::optional<claws::vect<float, 2>> nearest;

    for (int i = 0; i < 2; ++i)
      baseCornerHitbox[i] = unsigned(std::max(baseCornerHitboxF[i], 0.0f));
    for (unsigned i = baseCornerHitbox[0] ; i * tileSize < pos[0] + radius ; ++i)
      for (unsigned j = baseCornerHitbox[1] ; j * tileSize < pos[1] + radius ; ++j) {
	{
	  claws::vect<float, 2> collisionPoint{float(i), float(j)};

	  if (tile::isFullySolid(getTile({i, j})) && (pos - collisionPoint).length2() < (nearest ? (pos - *nearest).length2() : radius * radius))
	    nearest = collisionPoint;
	}
      }
    if (nearest)
      func(*nearest);
  }

};

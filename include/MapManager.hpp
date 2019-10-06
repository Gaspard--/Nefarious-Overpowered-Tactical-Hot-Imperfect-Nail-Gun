#pragma once

# include <vector>
# include <string>
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
  void collision(claws::vect<float, 2> pos, float radius, Func &&func)
  {
  }

};

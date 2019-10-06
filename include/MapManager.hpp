#pragma once

# include <vector>
# include <string>
# include "claws/container/vect.hpp"
# include "TileId.hpp"

class MapManager
{
  static const uint32_t tileSize;

  std::vector<std::vector<TileId>> mapTiles;
  claws::vect<int, 2> position{0, 0};
  claws::vect<int, 2> winSize;

  void generateChunk(uint32_t leftExp, uint32_t rightExp, uint32_t upExp, uint32_t downExp);
  void initTestMap();

public:

  MapManager(claws::vect<unsigned, 2> const &winSize);

  void moveMap(claws::vect<int, 2> const &movementSize);

  void fillDisplayData(claws::vect<int, 2> &mapOffset, claws::vect<int, 2> &mapSize, std::vector<TileId> &drawMap) const;
};

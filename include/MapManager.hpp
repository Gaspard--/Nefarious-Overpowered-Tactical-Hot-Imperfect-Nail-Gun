#pragma once

# include <vector>
# include <string>
# include "claws/container/vect.hpp"

enum class TileId : uint32_t
  {
   Empty,
   Wall,
   UpClosedWall,
   DownClosedWall,
   RightClosedWall,
   LeftClosedWall,
   Ground,
   Ceil,
  };

class MapManager
{

  static const uint32_t tileSize{32};

  std::vector<std::vector<TileId>> mapTiles;
  uint32_t position{0};
  int offset{0};
  bool infinite;
  claws::vect<unsigned, 2> winSize;

  void generateChunk(uint32_t sizeChunk);

public:

  MapManager(std::string const &fileName, claws::vect<unsigned, 2> const &winSize);     // load from file
  MapManager(uint32_t height, claws::vect<unsigned, 2> const &winSize); // generate

  void moveMap(int movementSize);

  void fillDisplayData(int &offset, std::vector<std::vector<TileId>> &drawMap) const;

};

# include "MapManager.hpp"
# include <cmath>

MapManager::MapManager(std::string const &fileName, claws::vect<unsigned, 2> const &winSize)
  : infinite(false)
  , winSize(winSize)
{
  //
  // file loading
  //
}

MapManager::MapManager(uint32_t height, claws::vect<unsigned, 2> const &winSize)
  : infinite(true)
  , winSize(winSize)
{
  mapTiles.resize(height);
  generateChunk(winSize[0] / tileSize * 2);
}

void MapManager::moveMap(int movementSize)
{
  offset += movementSize;
  int tileMovement = offset / tileSize;
  offset = offset % tileSize;

  if (tileMovement < 0 && abs(tileMovement) > position) {
    position = 0;
    offset = 0;
  } else {
    position += tileMovement;
    if (position + winSize[1] / tileSize > mapTiles[0].size()) {
      if (infinite)
	generateChunk(winSize[0] / tileSize * 2);
      else
	position = mapTiles[0].size() - winSize[1] / tileSize;
    }
  }
}

void MapManager::generateChunk(uint32_t sizeChunk)
{
  //
  // map gen
  //
}

void MapManager::fillDisplayData(int &offset, std::vector<std::vector<TileId>> &drawMap) const
{

}

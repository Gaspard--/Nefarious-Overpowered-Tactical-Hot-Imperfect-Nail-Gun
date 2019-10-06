# include "MapManager.hpp"
# include <cmath>

const uint32_t MapManager::tileSize = 32;

MapManager::MapManager(claws::vect<unsigned, 2> const &winSize)
  : winSize(winSize)
{
  generateChunk(0, winSize[0] / tileSize * 2, winSize[1] / tileSize * 2, 0);
  initTestMap();
}

void MapManager::initTestMap()
{
  std::vector<std::string> fakeMap;
  fakeMap.push_back("1111111111111111111111111111111111111111");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("2000000000000000000000000000000000000003");
  fakeMap.push_back("1111111111111111111111111111111111111111");
  mapTiles.resize(fakeMap.size());
  for (unsigned i = 0 ; i < fakeMap.size() ; ++i)
    for (unsigned j = 0 ; j < fakeMap[i].size() ; ++j)
      mapTiles[i].push_back((TileId)(fakeMap[i][j] - '0'));
}

void MapManager::moveMap(claws::vect<int, 2> const &movementSize)
{
  offset += movementSize;
  position += offset / tileSize;
  offset = offset % tileSize;
  if (position[0] + winSize[0] / tileSize > mapTiles[0].size())
    generateChunk(0, winSize[0] / tileSize * 2, 0, 0);
  else if (position[0] < 0)
    generateChunk(winSize[0] / tileSize * 2, 0, 0, 0);
  if (position[1] + winSize[1] / tileSize > mapTiles[1].size())
    generateChunk(0, 0, winSize[1] / tileSize * 2, 0);
  else if (position[1] < 0)
    generateChunk(0, 0, 0, winSize[1] / tileSize * 2);
}

void MapManager::generateChunk(uint32_t leftExp, uint32_t rightExp, uint32_t upExp, uint32_t downExp)
{
  (void)leftExp;
  (void)rightExp;
  (void)upExp;
  (void)downExp;
  //
  // map gen
  //
}

void MapManager::fillDisplayData(claws::vect<int, 2> &mapOffset, claws::vect<int, 2> &mapSize, std::vector<TileId> &drawMap) const
{
  mapOffset = offset;
  claws::vect<int, 2> drawPosition = position;
  if (offset[0] > 0)
    drawPosition[0] -= 1;
  if (offset[1] > 0)
    drawPosition[1] -= 1;
  mapSize = winSize / tileSize + 1;
  drawMap.resize(mapSize[0] * mapSize[1]);
  for (unsigned i = position[0] ; i != position[0] + mapSize[0]; ++i)
    for (unsigned j = position[1] ; j != position[1] + mapSize[1]; ++j) {
      if (i >= mapTiles.size() || j >= mapTiles[i].size())
	drawMap[i - position[0] + mapSize[0] * (j - position[1])] = TileId::Wall;
      else
	drawMap[i - position[0] + mapSize[0] * (j - position[1])] = mapTiles[i][j];
    }
}

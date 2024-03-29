#pragma once

#include "SpriteManager.hpp"
#include "TileId.hpp"

#include <vector>

struct AnimInfo
{
  claws::vect<float, 2u> destMin;
  claws::vect<float, 2u> destMax;
  uint32_t frame;
};

struct RotatedAnimInfo : public AnimInfo
{
  claws::vect<float, 2u> dir;
};

struct ColorInfo
{
  claws::vect<float, 2u> destMin;
  claws::vect<float, 2u> destMax;
  claws::vect<float, 4u> color;
};

static constexpr uint32_t bloodCount = 256;

struct DisplayData
{
  float timer;
  float heat;
  int score;
  std::string stringedTime;
  std::vector<ColorInfo> colors{};
  std::array<std::vector<AnimInfo>, size_t(SpriteId::SpriteCount)> anims;
  std::array<std::vector<RotatedAnimInfo>, size_t(SpriteId::SpriteCount)> rotatedAnims;
  float screenShake;
  bool gameOverHud{false};
  bool win{false};
  bool tuto{false};
  claws::vect<float, 2> offset;
  float zoom;
  claws::vect<int, 2> mapOffset;
  claws::vect<int, 2> mapSize;
  std::vector<TileId> mapData;
  std::vector<claws::vect<float, 2>> bloodPos;
  std::vector<claws::vect<float, 2>> bloodSpeed;
};

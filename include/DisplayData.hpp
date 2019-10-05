#pragma once

#include "SpriteManager.hpp"

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

struct DisplayData
{
  float timer;
  float heat;
  std::string stringedTime;
  std::vector<ColorInfo> colors{};
  std::array<std::vector<AnimInfo>, size_t(SpriteId::SpriteCount)> anims;
  std::array<std::vector<RotatedAnimInfo>, size_t(SpriteId::SpriteCount)> rotatedAnims;
  float screenShake;
  bool gameOverHud{false};
  bool win{false};
  bool tuto{false};
};

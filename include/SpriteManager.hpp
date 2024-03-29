#pragma once

#include <utility>

#include "opengl/my_opengl.hpp"
#include "SpriteId.hpp"

struct Animation
{
  opengl::Texture texture;
  uint32_t imageCount;
};

class SpriteManager
{
  std::array<Animation, static_cast<uint32_t>(SpriteId::SpriteCount)> animations;

  template<size_t... indices>
  SpriteManager(std::index_sequence<indices...>);

public:
  Animation const &operator[](SpriteId spriteId) const noexcept;

  SpriteManager();
};

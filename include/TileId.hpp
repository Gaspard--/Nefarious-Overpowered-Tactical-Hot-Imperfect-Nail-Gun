#pragma once

constexpr static float tileSize = 0.1f;

enum class TileId : uint32_t
  {
   Empty,
   Wall,
   Ground,
   Ceil,
   UpClosedWall,
   DownClosedWall,
   RightClosedWall,
   LeftClosedWall,
  };

namespace tile
{
  constexpr bool isFullySolid(TileId tile)
  {
    return tile == TileId::Wall ||
      tile == TileId::Ground ||
      tile == TileId::Ceil;
  }
}

#include "SpriteManager.hpp"
#include "opengl/load_image.hpp"
#include "loaders/load_image.hpp"

#include <cassert>
#include <utility>

inline Animation loadAnim(std::string const &path, uint32_t imageCount)
{
  if (path.substr(path.find_last_of(".") + 1) == "png")
    return {loaders::loadTexture(path.c_str()), imageCount};
  else
    return {opengl::loadTexture(path.c_str()), imageCount};
}

template<size_t index>
inline Animation getAnimation()
{
  switch (SpriteId(index))
    {
    case SpriteId::SmolWaspIdle:
      return loadAnim("resources/testWasp.bmp", 4);
    case SpriteId::WaspAbdomen:
      return loadAnim("resources/wasp abdomen.png", 1);
    case SpriteId::WaspBody:
      return loadAnim("resources/wasp body.png", 1);
    case SpriteId::WaspHead:
      return loadAnim("resources/wasp head.png", 1);
    case SpriteId::WaspAbdomenEnemy:
      return loadAnim("resources/wasp abdomen enemy.png", 1);
    case SpriteId::WaspBodyEnemy:
      return loadAnim("resources/wasp body enemy.png", 1);
    case SpriteId::WaspHeadEnemy:
      return loadAnim("resources/wasp head enemy.png", 1);
    case SpriteId::WaspLegs:
      return loadAnim("resources/wasp legs.bmp", 1); // TODO
    case SpriteId::WaspWing:
      return loadAnim("resources/wasp wings.png", 4);
    case SpriteId::Fireball :
      return loadAnim("resources/fireball-spriteSheet.bmp", 5);
    case SpriteId::Libeflux :
      return loadAnim("resources/libeflux-spriteSheet.bmp", 16);
    case SpriteId::Monarch :
      return loadAnim("resources/monarch-spriteSheet.bmp", 8);
    case SpriteId::SmolWasp :
      return loadAnim("resources/smolWasp-spriteSheet.bmp", 6);
    case SpriteId::Target:
      return loadAnim("resources/target.bmp", 1);
    case SpriteId::Blood:
      return loadAnim("resources/blood.png", 1);
    case SpriteId::Gore:
      return loadAnim("resources/Gore-spriteSheet.bmp", 3);
    case SpriteId::Back:
      return loadAnim("resources/back_1.bmp", 1);
    case SpriteId::DeadFellows:
      return loadAnim("resources/dead_screen.bmp", 1);
    case SpriteId::Boss:
      return loadAnim("resources/Boss.bmp", 1);
    case SpriteId::Tuto:
      return loadAnim("resources/tuto.bmp", 1);
    case SpriteId::Empty:
      return loadAnim("resources/empty.bmp", 1);
    case SpriteId::Wall:
      return loadAnim("resources/wall.png", 1);
    case SpriteId::Ground:
      return loadAnim("resources/ground.png", 1);
    case SpriteId::Ceil:
      return loadAnim("resources/ceil.bmp", 1);
    case SpriteId::UpClosedWall:
      return loadAnim("resources/upClosedWall.png", 1);
    case SpriteId::DownClosedWall:
      return loadAnim("resources/downClosedWall.png", 1);
    case SpriteId::LeftClosedWall:
      return loadAnim("resources/leftClosedWall.png", 1);
    case SpriteId::RightClosedWall:
      return loadAnim("resources/rightClosedWall.png", 1);
    case SpriteId::Nail:
      return loadAnim("resources/nail.png", 1);
    case SpriteId::NailGun:
      return loadAnim("resources/nail gun.png", 1);
    default:
      assert("missing animation definition");
    }
}

template<size_t... indices>
SpriteManager::SpriteManager(std::index_sequence<indices...>)
  : animations{getAnimation<indices>()...}
{
}

Animation const &SpriteManager::operator[](SpriteId spriteId) const noexcept
{
  return animations[uint32_t(spriteId)];
}

SpriteManager::SpriteManager()
  : SpriteManager(std::make_index_sequence<size_t(SpriteId::SpriteCount)>())
{
}

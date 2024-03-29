#include "Display.hpp"
#include "Logic.hpp"
#include "opengl/RenderContext.hpp"
#include "Bind.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>

inline opengl::RenderContext contextFromFiles(std::string name)
{
  std::stringstream vert;
  std::stringstream frag;
  {
    std::ifstream vertInput("shaders/" + name + ".vert");
    std::ifstream fragInput("shaders/" + name + ".frag");

    if (!fragInput || !vertInput)
      {
	std::cout << "shaders/" + name + ".vert" << std::endl;
	std::cout << "shaders/" + name + ".frag" << std::endl;
	throw std::runtime_error("Failed to load shaders");
      }
    vert << vertInput.rdbuf();
    frag << fragInput.rdbuf();
  }
  return {opengl::Vao(), opengl::createProgram<2>({{opengl::createShader(static_cast<unsigned int>(GL_VERTEX_SHADER), vert.str().c_str()),
					            opengl::createShader(static_cast<unsigned int>(GL_FRAGMENT_SHADER), frag.str().c_str())}})};
}

Display::Display(GLFWwindow &window)
  : window(&window)
  , textureContext(contextFromFiles("texture"))
  , rectContext(contextFromFiles("rect"))
  , textContext(contextFromFiles("text"))
  , bulletContext(contextFromFiles("bullet"))
  , bloodContext(contextFromFiles("blood"))
  , fontHandler("./resources/FantasqueSansMono-Regular.ttf")
{
  {
    Bind bind(textureContext);

    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    uint32_t attrib0 = textureContext.program.getAttribLocation("pos");
    uint32_t attrib1 = textureContext.program.getAttribLocation("coord");
    glEnableVertexAttribArray(attrib0);
    glEnableVertexAttribArray(attrib1);
    glVertexAttribPointer(attrib0, 2, GL_FLOAT, false, 4 * sizeof(float), nullptr);
    glVertexAttribPointer(attrib1, 2, GL_FLOAT, false, 4 * sizeof(float), reinterpret_cast<void *>(2u * sizeof(float)));
  }
  {
    Bind bind(bulletContext);

    glBindBuffer(GL_ARRAY_BUFFER, bulletBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    uint32_t attrib0 = bulletContext.program.getAttribLocation("pos");
    uint32_t attrib1 = bulletContext.program.getAttribLocation("color");
    glEnableVertexAttribArray(attrib0);
    glEnableVertexAttribArray(attrib1);
    glVertexAttribPointer(attrib0, 2, GL_FLOAT, false, 6 * sizeof(float), nullptr);
    glVertexAttribPointer(attrib1, 4, GL_FLOAT, false, 6 * sizeof(float), reinterpret_cast<void *>(2u * sizeof(float)));
  }
  {
    Bind bind(rectContext);

    glBindBuffer(GL_ARRAY_BUFFER, rectBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    uint32_t attrib0 = rectContext.program.getAttribLocation("pos");
    glEnableVertexAttribArray(attrib0);
    glVertexAttribPointer(attrib0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);
  }
  {
    Bind bind(textContext);

    glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    uint32_t attrib0 = textContext.program.getAttribLocation("pos");
    uint32_t attrib1 = textContext.program.getAttribLocation("coord");
    glEnableVertexAttribArray(attrib0);
    glEnableVertexAttribArray(attrib1);
    glVertexAttribPointer(attrib0, 2, GL_FLOAT, false, 4 * sizeof(float), nullptr);
    glVertexAttribPointer(attrib1, 2, GL_FLOAT, false, 4 * sizeof(float), reinterpret_cast<void *>(2u * sizeof(float)));
  }
  {
    glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);

    std::array<float, 24> data{{0.0f, 0.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				0.0f, 1.0f,
				1.0f, 1.0f,
				1.0f, 1.0f}};
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
  }
  {
    Bind bind(bloodContext);

    glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);

    uint32_t attrib0 = textContext.program.getAttribLocation("pos");
    uint32_t attrib1 = textContext.program.getAttribLocation("coord");
    glEnableVertexAttribArray(attrib0);
    glEnableVertexAttribArray(attrib1);
    glVertexAttribPointer(attrib0, 2, GL_FLOAT, false, 4 * sizeof(float), nullptr);
    glVertexAttribPointer(attrib1, 2, GL_FLOAT, false, 4 * sizeof(float), reinterpret_cast<void *>(2u * sizeof(float)));
  }
  glBindBuffer(GL_ARRAY_BUFFER, bloodBuffer);
  glBufferData(GL_ARRAY_BUFFER, bloodCount * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, bloodSpeedBuffer);
  glBufferData(GL_ARRAY_BUFFER, bloodCount * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
}

void Display::renderSingleAnim(AnimInfo const &anim, SpriteId spriteId)
{
  {
    Bind bind(textureContext);

    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

    std::array<float, 6 * 4> data;

    std::array<float, 12> corner{{0.0f, 0.0f,
				  1.0f, 0.0f,
				  0.0f, 1.0f,
				  1.0f, 0.0f,
				  0.0f, 1.0f,
				  1.0f, 1.0f}};

    for (uint32_t i(0u); i != 6; ++i)
      {
	for (uint32_t j(0u); j != 2; ++j)
	  data[i * 4 + j] = (anim.destMax[j] * corner[i * 2 + j] + anim.destMin[j] * (1.0f - corner[i * 2 + j]));
	data[i * 4 + 2] = (corner[i * 2]);
	data[i * 4 + 3] = ((corner[i * 2 + 1] + float(anim.frame)) / float(spriteManager[spriteId].imageCount));
      }
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    opengl::setUniform(dim, "dim", textureContext.program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spriteManager[spriteId].texture);
    opengl::setUniform(0, "tex", textureContext.program);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

void Display::renderBack(SpriteId sprite, claws::vect<float, 2u> offset, float zoom)
{
  {
    Bind bind(textureContext);

    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

    std::array<float, 6 * 4> data;

    std::array<float, 12> corner{{-1.0f, -1.0f,
				  1.0f, -1.0f,
				  -1.0f, 1.0f,
				  1.0f, -1.0f,
				  -1.0f, 1.0f,
				  1.0f, 1.0f}};

    for (uint32_t i(0u); i != 6; ++i)
      {
	for (uint32_t j(0u); j != 2; ++j)
	  data[i * 4 + j] = (corner[i * 2 + j]) * dim[j];
	data[i * 4 + 2] = ((corner[i * 2]) * dim[0]) / zoom - offset[0];
	data[i * 4 + 3] = ((corner[i * 2 + 1]) * dim[1]) / zoom - offset[1];
      }
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    opengl::setUniform(dim, "dim", textureContext.program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spriteManager[sprite].texture);
    opengl::setUniform(0, "tex", textureContext.program);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

void Display::renderText(std::string const &text, unsigned int fontSize, claws::vect<float, 2u> step, claws::vect<float, 2u> textPos, claws::vect<float, 3u> color)
{
  fontHandler.renderText(text, [this, textPos, color](claws::vect<float, 2u> pen, claws::vect<float, 2u> size, unsigned char *buffer, claws::vect<int, 2u> fontDim)
			       {
				 opengl::Texture texture;
				 Bind bind(textContext);

				 glActiveTexture(GL_TEXTURE0);
				 glBindTexture(GL_TEXTURE_2D, texture);
				 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				 glTexImage2D(GL_TEXTURE_2D,
					      0,
					      GL_RED,
					      fontDim[0],
					      fontDim[1],
					      0,
					      GL_RED,
					      GL_UNSIGNED_BYTE,
					      static_cast<void *>(buffer));
				 float data[16];

				 for (unsigned int i(0); !(i & 4u); ++i)
				   {
				     claws::vect<float, 2u> corner{static_cast<float>(i & 1u), static_cast<float>(i >> 1u)};
				     claws::vect<float, 2u> destCorner(pen + textPos + corner * size);

				     data[i * 4 + 0] = destCorner[0];
				     data[i * 4 + 1] = destCorner[1];
				     data[i * 4 + 2] = corner[0];
				     data[i * 4 + 3] = corner[1];
				   }
				 glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
				 glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				 opengl::setUniform(dim, "dim", textContext.program);
				 opengl::setUniform(color, "textColor", textContext.program);
				 opengl::setUniform(0, "tex", textContext.program);
				 glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			       }, fontSize, step);
}

void Display::renderColors(std::vector<ColorInfo> const &colorInfos)
{
  {
    Bind bind(bulletContext);

    glBindBuffer(GL_ARRAY_BUFFER, bulletBuffer);

    std::vector<float> data;

    data.reserve(colorInfos.size() * 6 * 6);

    for (auto const &colorInfo : colorInfos)
      {
	std::array<float, 12> corner{{0.0f, 0.0f,
				      1.0f, 0.0f,
				      0.0f, 1.0f,
				      1.0f, 0.0f,
				      0.0f, 1.0f,
				      1.0f, 1.0f}};

	for (uint32_t i(0u); i != 6; ++i)
	  {
	    for (uint32_t j(0u); j != 2; ++j)
	      data.emplace_back(colorInfo.destMax[j] * corner[i * 2 + j] + colorInfo.destMin[j] * (1.0f - corner[i * 2 + j]));
	    for (uint32_t j(0u); j != 4; ++j)
	      data.emplace_back(colorInfo.color[j]);
	  }
      }
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    opengl::setUniform(dim, "dim", bulletContext.program);
    glDrawArrays(GL_TRIANGLES, 0, uint32_t(colorInfos.size() * 6));
  }
}

void Display::renderAnims(std::vector<AnimInfo> const &anims, SpriteId spriteId)
{
  {
    Bind bind(textureContext);

    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

    std::vector<float> data;

    data.reserve(anims.size() * 6 * 4);

    for (auto const &anim : anims)
      {
	std::array<float, 12> corner{{0.0f, 0.0f,
				      1.0f, 0.0f,
				      0.0f, 1.0f,
				      1.0f, 0.0f,
				      0.0f, 1.0f,
				      1.0f, 1.0f}};

	for (uint32_t i(0u); i != 6; ++i)
	  {
	    for (uint32_t j(0u); j != 2; ++j)
	      data.emplace_back(anim.destMax[j] * corner[i * 2 + j] + anim.destMin[j] * (1.0f - corner[i * 2 + j]));
	    data.emplace_back(corner[i * 2]);
	    data.emplace_back((corner[i * 2 + 1] + float(anim.frame)) / float(spriteManager[spriteId].imageCount));
	  }
      }
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    opengl::setUniform(dim, "dim", textureContext.program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spriteManager[spriteId].texture);
    opengl::setUniform(0, "tex", textureContext.program);
    glDrawArrays(GL_TRIANGLES, 0, uint32_t(anims.size() * 6));
  }
}

void Display::renderRotatedAnims(std::vector<RotatedAnimInfo> const &rotatedAnims, SpriteId spriteId)
{
  {
    Bind bind(textureContext);

    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

    std::vector<float> data;

    data.reserve(rotatedAnims.size() * 6 * 4);

    for (auto const &rotatedAnim : rotatedAnims)
      {
	claws::vect<float, 2> dir{rotatedAnim.dir / std::sqrt(rotatedAnim.dir.length2())};
	claws::vect<float, 2> center((rotatedAnim.destMin + rotatedAnim.destMax) * 0.5f);
	std::array<float, 12> corner{{0.0f, 0.0f,
				      1.0f, 0.0f,
				      0.0f, 1.0f,
				      1.0f, 0.0f,
				      0.0f, 1.0f,
				      1.0f, 1.0f}};


	for (uint32_t i(0u); i != 6; ++i)
	  {
	    auto position(rotatedAnim.destMax * claws::vect<float, 2u>{corner[i * 2], corner[i * 2 + 1]} +
			  rotatedAnim.destMin * claws::vect<float, 2u>{1.0f - corner[i * 2], 1.0f - corner[i * 2 + 1]});

	    position -= center;
	    position = (position * dir[1] + claws::vect<float, 2>{position[1], -position[0]} * dir[0]);
	    position += center;
	    for (uint32_t j(0u); j != 2; ++j)
	      data.emplace_back(position[j]);
	    data.emplace_back(corner[i * 2]);
	    data.emplace_back((corner[i * 2 + 1] + float(rotatedAnim.frame)) / float(spriteManager[spriteId].imageCount));
	  }
      }
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    opengl::setUniform(dim, "dim", textureContext.program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spriteManager[spriteId].texture);
    opengl::setUniform(0, "tex", textureContext.program);
    glDrawArrays(GL_TRIANGLES, 0, uint32_t(rotatedAnims.size() * 6));
  }
}


void Display::renderHud(float bigWaspSize, uint32_t score, float heat, std::string const &strTime, float)
{
  (void)score;
  (void)bigWaspSize;
  //renderText("  Size  : " + std::to_string(uint32_t(bigWaspSize * 1000.0f)), 400, {0.05f, 0.05f}, {1.0f, 0.855f}, {1.0f, 1.0f, 1.0f});
  //renderText("  Hps   : " + std::to_string(666), 400, {0.05f, 0.05f}, {1.0f, 0.755f}, {1.0f, 1.0f, 1.0f});
  renderText("  Score : " + std::to_string(score), 400, {0.05f, 0.05f}, {1.0f, 0.655f}, {1.0f, 1.0f, 1.0f});
  std::string progress_bar{"["};
  for (unsigned i = 0 ; i < 10 ; ++i) {
    if (float(i) < heat * 10.f)
      progress_bar.push_back('>');
    else
      progress_bar.push_back(' ');
  }
  progress_bar.push_back(']');
  renderText("  Gun heat : ", 400, {0.05f, 0.05f}, {1.0f, 0.855f}, {1.0f, 1.0f, 1.0f});
  renderText(progress_bar, 400, {0.05f, 0.05f}, {1.35f, 0.855f}, {1.0f, 1.0f - heat, 1.0f - heat});
  renderText("  Time  : " + strTime, 400, {0.05f, 0.05f}, {1.0f, 0.755f}, {1.0f, 1.0f, 1.0f});
  // auto secondTime((uint32_t(timer) * Logic::getTickTime().count()) / 1000000);
  // std::string inGameTime;

  // if (secondTime / 60 >= 10)
  //   inGameTime = std::to_string(secondTime / 60) + " m ";
  // else if (secondTime / 60)
  //   inGameTime = "0" + std::to_string(secondTime / 60) + " m ";
  // if ((secondTime) % 60 >= 10)
  //   inGameTime += std::to_string((secondTime) % 60) + " s";
  // else
  //   inGameTime += "0" + std::to_string((secondTime) % 60) + " s";
  // renderText("  In game time : " + inGameTime, 400, {0.05f, 0.05f}, {1.0f, 0.455f}, {1.0f, 1.0f, 1.0f});

}

void Display::renderGameOver(uint32_t score, std::string const &strTime, bool win)
{
  renderColors({{claws::vect<float, 2u>(-2.0f, 1.0f), claws::vect<float, 2u>(2.0f, -1.0f),
	  win ? claws::vect<float, 4u>{0.06f, 0.06f, 0.04f, 0.4f}
	: claws::vect<float, 4u>{0.5f, 0.01f, 0.04f, 0.4f}}});
  renderText(win ? " You Win" : "Game Over", 300, {0.07f, 0.07f}, {-0.18f, 0.25f}, {1.0f, 1.0f, 1.0f});
  renderText("Final Time  " + strTime, 200, {0.05f, 0.05f}, {-0.18f, 0.05f}, {1.0f, 1.0f, 1.0f});
  renderText("Final Score " + std::to_string(score), 200, {0.05f, 0.05f}, {-0.18f, -0.05f}, {1.0f, 1.0f, 1.0f});
}

void Display::renderDeadScreen(const std::vector<std::pair<std::string, std::string>>& fellows)
{
  renderSingleAnim({{-1.75f, 0.65f}, {-1.03f, 0.97f}, 0}, SpriteId::DeadFellows);
  float y = 0.55f;
  for (auto i = fellows.begin() ; i != fellows.end() ; ++i) {
    renderText(i->first + " " + i->second, 200, {0.05f, 0.05f}, {-1.65f, y}, {1.0f, 1.0f, 1.0f});
    y -= 0.05f;
    if (y <= -1.0f)
      break;
  }
}

void Display::renderTerrain(DisplayData const &displayData)
{
  for (int x = 0; x != displayData.mapSize[0] * displayData.mapSize[1]; ++x)
    {
      int x2 = x + 1;
      for (; (x2 % displayData.mapSize[0]) != 0; ++x2)
	if (displayData.mapData[x2] != displayData.mapData[x])
	  break;
      SpriteId sprite;
      switch (displayData.mapData[x]) {
      case TileId::Wall:
  	sprite = SpriteId::Wall;
  	break;
      case TileId::Empty:
  	continue;
      case TileId::Ceil:
  	sprite = SpriteId::Ceil;
  	break;
      case TileId::Ground:
  	sprite = SpriteId::Ground;
  	break;
      case TileId::UpClosedWall:
  	sprite = SpriteId::UpClosedWall;
  	break;
      case TileId::DownClosedWall:
  	sprite = SpriteId::DownClosedWall;
  	break;
      case TileId::LeftClosedWall:
  	sprite = SpriteId::LeftClosedWall;
  	break;
      case TileId::RightClosedWall:
  	sprite = SpriteId::RightClosedWall;
  	break;
      default:
  	break;
      }
      claws::vect<int, 2u> tilePos(x % displayData.mapSize[0], x / displayData.mapSize[0]);

      tilePos += displayData.mapOffset;
      auto pos(claws::vect_cast<float>(tilePos));
      pos *= tileSize;
      pos += displayData.offset;
      auto min(pos * displayData.zoom);
      auto max((pos + claws::vect<float, 2u>(float(x2 - x), 1.0f) * tileSize) * displayData.zoom);
      {
	Bind bind(textureContext);

	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

	std::array<float, 6 * 4> data;

	std::array<float, 12> corner{{0.0f, 0.0f,
				      1.0f, 0.0f,
				      0.0f, 1.0f,
				      1.0f, 0.0f,
				      0.0f, 1.0f,
				      1.0f, 1.0f}};

	for (uint32_t i(0u); i != 6; ++i)
	  {
	    for (uint32_t j(0u); j != 2; ++j)
	      data[i * 4 + j] = (max[j] * corner[i * 2 + j] + min[j] * (1.0f - corner[i * 2 + j]));
	    data[i * 4 + 2] = (corner[i * 2] * float(x2 - x));
	    data[i * 4 + 3] = (corner[i * 2 + 1]);
	  }
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	opengl::setUniform(dim, "dim", textureContext.program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spriteManager[sprite].texture);
	opengl::setUniform(0, "tex", textureContext.program);
	glDrawArrays(GL_TRIANGLES, 0, 6);
      }
      x = x2 - 1;
    }
}

void Display::renderBlood(DisplayData const &data)
{
  Bind bind(bloodContext);

  glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);

  glBindBuffer(GL_UNIFORM_BUFFER, bloodBuffer);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, data.bloodPos.size() * sizeof(float) * 2, data.bloodPos.data());
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, bloodBuffer, 0, data.bloodPos.size() * sizeof(float) * 2);

  glBindBuffer(GL_UNIFORM_BUFFER, bloodSpeedBuffer);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, data.bloodSpeed.size() * sizeof(float) * 2, data.bloodSpeed.data());
  glBindBufferRange(GL_UNIFORM_BUFFER, 1, bloodSpeedBuffer, 0, data.bloodSpeed.size() * sizeof(float) * 2);

  opengl::setUniform(dim, "dim", bloodContext.program);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, spriteManager[SpriteId::Blood].texture);
  opengl::setUniform(0, "tex", bloodContext.program);

  opengl::setUniform(data.zoom, "zoom", bloodContext.program);
  opengl::setUniform(data.offset, "offset", bloodContext.program);

  assert(glGetUniformBlockIndex(bloodContext.program, "BloodPosition") != GL_INVALID_INDEX);
  glUniformBlockBinding(bloodContext.program, glGetUniformBlockIndex(bloodContext.program, "BloodPosition"), 0);

  assert(glGetUniformBlockIndex(bloodContext.program, "BloodSpeed") != GL_INVALID_INDEX);
  glUniformBlockBinding(bloodContext.program, glGetUniformBlockIndex(bloodContext.program, "BloodSpeed"), 1);

  assert(glGetError() != GL_TRUE);

  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, bloodCount);
}

void Display::render(DisplayData const &data)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //do final render here
  glClearColor(0.3f, 0.2f, 0.2f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  renderBack(SpriteId::Wall, data.offset, data.zoom * 0.1f);
  renderBack(SpriteId::UpClosedWall, data.offset, data.zoom * 0.2f);
  renderColors({{claws::vect<float, 2u>(-dim[0], dim[1]), claws::vect<float, 2u>(dim[0], -dim[1]), claws::vect<float, 4u>{data.screenShake * 0.01f, data.screenShake * 0.01f, 0.04f, 0.8f}}});
  renderTerrain(data);
  renderBlood(data);
  for (size_t i(0u); i < data.anims.size(); ++i)
    if (!data.anims[i].empty())
      renderAnims(data.anims[i], SpriteId(i));
  for (size_t i(0u); i < data.rotatedAnims.size(); ++i)
    if (!data.rotatedAnims[i].empty())
      renderRotatedAnims(data.rotatedAnims[i], SpriteId(i));
  if (!data.colors.empty())
    renderColors(data.colors);
  // renderColors({{-dim, claws::vect<float, 2u>(-1.0f, 1.0f), claws::vect<float, 4u>{0.0f, 0.0f, 0.0f, 1.0f}},
  // 		{dim, claws::vect<float, 2u>(1.0f, -1.0f), claws::vect<float, 4u>{0.0f, 0.0f, 0.0f, 1.0f}}});
  renderHud(666.0f, data.score, data.heat, data.stringedTime, data.timer);
  if (data.gameOverHud)
    renderGameOver(data.score, data.stringedTime, data.win);
  if (data.tuto)
    renderSingleAnim(AnimInfo{claws::vect<float, 2u>{-1.980f / 1.080f, -1.0f},
			      claws::vect<float, 2u>{1.980f / 1.080f, 1.0f},
			      0}, SpriteId::Tuto);

}

void Display::resize(claws::vect<uint32_t, 2u> size)
{
  this->size = size;
  this->dim = {float(size[0]) / float(size[1]), 1.0f};
  glViewport(0, 0, size[0], size[1]);
}

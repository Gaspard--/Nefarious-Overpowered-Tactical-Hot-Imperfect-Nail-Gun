#include "loaders/load_image.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <cstdio>
#include <png.h>
#include <cstring>
#include <utility>
#include <iostream>
#include <cassert>

#include <claws/utils/on_scope_exit.hpp>

namespace loaders
{
  namespace
  {
    uint32_t bytesToInt(char const *bytes)
    {
      return (static_cast<unsigned char>(bytes[3]) << 24u)
	| (static_cast<unsigned char>(bytes[2]) << 16u)
	| (static_cast<unsigned char>(bytes[1]) << 8u)
	| static_cast<unsigned char>(bytes[0]);
    }

    opengl::Texture convertToTexture(char const *data, std::array<unsigned int, 2u> const &dim)
    {
      opengl::Texture texture;

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   static_cast<GLsizei>(dim[0]),
                   static_cast<GLsizei>(dim[1]),
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   data);
      glBindTexture(GL_TEXTURE_2D, 0);
      return texture;
    }

    std::pair<std::unique_ptr<char[]>, std::array<unsigned int, 2u>> loadTextureFromBmp(char const *name)
    {
      std::ifstream file(name, std::ios::binary);

      if (!file)
	throw std::runtime_error("'" + std::string(name) + "': failed to open");
      file.exceptions(std::ios::badbit);

      char readBuf[4];
      std::array<unsigned int, 2u> dim{0u, 0u};
      try {
	file.seekg(10);
	file.read(readBuf, sizeof(readBuf));
	unsigned int offset(bytesToInt(readBuf));

	file.seekg(14);
	file.read(readBuf, sizeof(readBuf));

	file.read(readBuf, sizeof(readBuf));
	dim[0] = bytesToInt(readBuf);

	file.read(readBuf, sizeof(readBuf));
	dim[1] = bytesToInt(readBuf);

	file.seekg(offset);

	std::unique_ptr<char[]> data(new char[dim[0] * dim[1] * sizeof(uint32_t)]);

	file.read(&data[0], std::streamsize(dim[0] * dim[1] * sizeof(uint32_t)));

	std::streamsize r(file.gcount());

	if (r != std::streamsize(dim[0] * dim[1] * sizeof(uint32_t)))
	  {
	    std::stringstream s;

	    s << name << ": file seems truncated, " << r << std::string(" bytes read. Expected ") << std::streamsize(dim[0] * dim[1] * sizeof(uint32_t));
	    throw std::runtime_error(s.str());
	  }
	file.exceptions(std::ios::goodbit);

	for (auto it = &data[0]; it < &data[dim[0] * dim[1] * sizeof(uint32_t)]; it += sizeof(uint32_t))
	  {
	    std::swap(it[0], it[3]);
	    std::swap(it[1], it[2]);
	  }
	return {std::move(data), dim};
      } catch (std::exception const &e) {
	std::stringstream s;

	s << name << ": failed to load texture(" << e.what() << ")";
	throw std::runtime_error(s.str());
      }
    }

    std::pair<std::unique_ptr<char[]>, std::array<unsigned int, 2u>> loadTextureFromPng(char const *name)
    {
      try {
	std::unique_ptr<FILE, decltype(&fclose)> fp(fopen(name, "r"), &fclose);

	if (!fp)
	  throw std::runtime_error("error while opening file");
	unsigned char header[8];

	fread(header, 1, 8, fp.get());
	if (png_sig_cmp(header, 0, 8))
	  throw std::runtime_error("bad file");

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	  throw std::runtime_error("error when creating read struct");
	claws::on_scope_exit guard([&png_ptr]()
				   {
				     png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
				   });

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	  throw std::runtime_error("error when creating info struct");
	claws::on_scope_exit guard2([png_ptr, &info_ptr]()
				    {
				      png_destroy_info_struct(png_ptr, &info_ptr);
				    });

	if (setjmp(png_jmpbuf(png_ptr)))
	  throw std::runtime_error("[read_png_file] Error during init_io");

	png_init_io(png_ptr, fp.get());
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	std::array<unsigned int, 2u> dim{0u, 0u};
	dim[0] = png_get_image_width(png_ptr, info_ptr);
	dim[1] = png_get_image_height(png_ptr, info_ptr);

	png_byte color_type = png_get_color_type(png_ptr, info_ptr); // not used but maybe usefull
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr); // idem

	png_set_interlace_handling(png_ptr);
	if (bit_depth == 16)
	  {
	    png_set_scale_16(png_ptr);
	  }
	if (color_type == PNG_COLOR_TYPE_GRAY ||
	    color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	  {
	    png_set_gray_to_rgb(png_ptr);
	  }
	if (color_type == PNG_COLOR_TYPE_RGB ||
	    color_type == PNG_COLOR_TYPE_GRAY ||
	    color_type == PNG_COLOR_TYPE_PALETTE)
	  png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
	  throw std::runtime_error("[read_png_file] Error during read_image");

	std::unique_ptr<png_bytep[]> row_pointers(new png_bytep[dim[1]]);
	size_t rowSize = png_get_rowbytes(png_ptr,info_ptr);
	std::unique_ptr<char[]> data(new char[rowSize * dim[1]]);
	for (unsigned y = 0; y < dim[1] ; y++)
	  row_pointers[dim[1] - 1 - y] = reinterpret_cast<unsigned char*>(data.get()) + y * rowSize;

	png_read_image(png_ptr, row_pointers.get());
	// dumpRawTexture(data.get(), dim);

	return {std::move(data), dim};
      } catch (std::exception const &e) {
	std::stringstream s;

	s << name << ": failed to load texture(" << e.what() << ")";
	throw std::runtime_error(s.str());
      }
    }

    std::pair<std::unique_ptr<char[]>, std::array<unsigned int, 2u>> loadRawTexture(char const *name_)
    {
      std::string name(name_);

      if (!name.compare(name.length() - 4, 4, ".png")) {
	return loadTextureFromPng(name.c_str());
      } else if (!name.compare(name.length() - 4, 4, ".bmp")) {
	return loadTextureFromBmp(name.c_str());
      } else
	throw std::runtime_error(name + ": unknow file type");
    }
  }

  opengl::Texture loadTexture(char const *name)
  {
    auto [data, dim] = loadRawTexture(name);

    return convertToTexture(data.get(), dim);
  }
}

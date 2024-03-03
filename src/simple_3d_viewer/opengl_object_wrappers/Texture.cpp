#include "simple_3d_viewer/utils/fileOperations.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <fmt/format.h>
#include <iostream>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/utils/Image.hpp>
#include <stb_image.h>

namespace Simple3D
{

void Texture::init()
{
  loadTextures();
  if (type_ == Type::Texture2D)
  {
    createTexture2D();
  }
  else if (type_ == Type::TextureCubeMap)
  {
    createTextureCubeMap();
  }
  else
  {
    assert(false);
  }
}

void Texture::loadData()
{
  loadTextures();
}

void Texture::loadTextures()
{
  bool flipVertically = false;
  if (type_ == Type::Texture2D)
  {
    flipVertically = true;
  }

  for (const auto& path : paths_)
  {
    loadedImages_.push_back(loadImage(path, flipVertically));
  }
}

void Texture::createTexture2D()
{
  glGenTextures(1, &textureHandle_);
  glBindTexture(GL_TEXTURE_2D, textureHandle_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const auto& image = loadedImages_.back();
  const auto colorChannelsCount = image.colorChannelCount;
  const auto [width, height] = image.size;

  GLenum format{};
  if (colorChannelsCount == 4)
  {
    format = GL_RGBA;
  }
  else if (colorChannelsCount == 3)
  {
    format = GL_RGB;
  }
  else if (colorChannelsCount == 1)
  {
    format = GL_RED;
  }
  else
  {
    throw std::invalid_argument(fmt::format(
        "Invalid number of color channels for texture at path {}.",
        paths_[0].c_str()));
  }

  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      static_cast<GLint>(format),
      width,
      height,
      0,
      format,
      GL_UNSIGNED_BYTE,
      image.image);

  loadedImages_.clear();
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::createTextureCubeMap()
{
  glGenTextures(1, &textureHandle_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle_);

  for (int i = 0; i < loadedImages_.size(); ++i)
  {
    const auto& image = loadedImages_[i];
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        0,
        GL_RGB,
        image.size.width,
        image.size.height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        image.image);
  }
  loadedImages_.clear();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

}  // namespace Simple3D

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <stb_image.h>

namespace Simple3D
{
void Texture::init()
{
  if (type_ == Type::Texture2D)
  {
    loadTexture2DData();
    createTexture2D();
  }
  else if (type_ == Type::TextureCubeMap)
  {
    loadTextureCubeMapData();
    createTextureCubeMap();
  }
  else
    assert(false);
}

void Texture::loadData()
{
  if (type_ == Type::Texture2D)
    loadTexture2DData();
  else if (type_ == Type::TextureCubeMap)
    loadTextureCubeMapData();
  else
    assert(false);
}

void Texture::loadTexture2DData()
{
  loadedImages_.push_back(loadImageAtPath(0));
}

void Texture::loadTextureCubeMapData()
{
  for (int i = 0; i < paths_.size(); ++i)
    loadedImages_.push_back(loadImageAtPath(i, false));
}

uint8_t* Texture::loadImageAtPath(int pathsIndex, bool flip)
{
  if (flip)
  {
    stbi_set_flip_vertically_on_load(1);
  }
  uint8_t* loadedImage = stbi_load(
      (paths_[pathsIndex]).c_str(),
      &widths_[pathsIndex],
      &heights_[pathsIndex],
      &colorChannelsCounts_[pathsIndex],
      0);
  if (loadedImage == nullptr)
  {
    const auto errorMessage =
        "Failed to load image at path: " + paths_[pathsIndex].string();
    throw std::invalid_argument(errorMessage);
  }
  return loadedImage;
}

void Texture::createTexture2D()
{
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int colorChannelsCount = colorChannelsCounts_.back();
  GLenum format{};
  if (colorChannelsCount == 4)
    format = GL_RGBA;
  else if (colorChannelsCount == 3)
    format = GL_RGB;
  else if (colorChannelsCount == 1)
    format = GL_RED;
  else
  {
    const auto errorMessage =
        "Invalid number of color channels for texture at path: " +
        paths_[0].string();
    throw std::invalid_argument(errorMessage);
  }

  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      format,
      widths_.front(),
      heights_.front(),
      0,
      format,
      GL_UNSIGNED_BYTE,
      loadedImages_.back());

  stbi_image_free(loadedImages_.back());
  loadedImages_.clear();
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::createTextureCubeMap()
{
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);

  for (int i = 0; i < loadedImages_.size(); ++i)
  {
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        0,
        GL_RGB,
        widths_[i],
        heights_[i],
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        loadedImages_[i]);
    stbi_image_free(loadedImages_[i]);
  }
  loadedImages_.clear();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
}  // namespace Simple3D

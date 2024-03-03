#include "simple_3d_viewer/utils/fileOperations.hpp"
#include <stdexcept>
#include <tl/expected.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <fmt/format.h>
#include <iostream>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/utils/Image.hpp>
#include <stb_image.h>

namespace Simple3D
{

namespace
{

tl::expected<GLuint, std::string> createTexture2D(const Image& image)
{
  GLuint textureHandle{};
  glGenTextures(1, &textureHandle);
  glBindTexture(GL_TEXTURE_2D, textureHandle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
    return tl::make_unexpected("Invalid number of color channels!");
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

  glBindTexture(GL_TEXTURE_2D, 0);
  return textureHandle;
}

GLuint createTextureCubeMap(const std::vector<Image>& images)
{
  GLuint textureHandle{};
  glGenTextures(1, &textureHandle);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);

  for (int i = 0; i < images.size(); ++i)
  {
    const auto& image = images[i];
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

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureHandle;
}

}  // namespace

void Texture::init()
{
  loadTextures();
  complete();
}

void Texture::complete()
{
  assert(textureHandle_ != 0);
  if (type_ == Type::Texture2D)
  {
    textureHandle_ =
        *createTexture2D(loadedImages_.front())
             .or_else(
                 [&path = paths_.front()](const auto& error)
                 {
                   throw std::invalid_argument(fmt::format(
                       "Error: {} For texture at path: {}",
                       error,
                       path.c_str()));
                   return tl::expected<GLuint, std::string>(GLuint{});
                 });
    loadedImages_.clear();
  }
  else if (type_ == Type::TextureCubeMap)
  {
    textureHandle_ = createTextureCubeMap(loadedImages_);
    loadedImages_.clear();
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

void Texture::release()
{
  if (textureHandle_ == 0)
  {
    return;
  }

  glDeleteTextures(1, &textureHandle_);
  textureHandle_ = 0;
}

void Texture::use(const uint32_t textureSlot)
{
  glActiveTexture(GL_TEXTURE0 + textureSlot);
  if (type_ == Type::Texture2D)
  {
    glBindTexture(GL_TEXTURE_2D, textureHandle_);
  }
  else if (type_ == Type::TextureCubeMap)
  {
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle_);
  }
}

}  // namespace Simple3D

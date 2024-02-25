#pragma once

#include <glad/glad.h>

#include <assert.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace Simple3D
{
class Texture
{
  enum class Type
  {
    Texture2D,
    TextureCubeMap
  };

 public:
  Texture() = delete;
  Texture(const std::string &path, bool doOpenGLStuff = true)
      : type_(Type::Texture2D)
  {
    const int imagesCount = 1;
    paths_.emplace_back(path);
    widths_.resize(imagesCount);
    heights_.resize(imagesCount);
    colorChannelsCounts_.resize(1);
    if (doOpenGLStuff)
      init();
    else
      loadData();
  }
  Texture(const std::vector<std::string> &paths, bool doOpenGLStuff = true)
      : paths_(paths),
        type_(Type::TextureCubeMap)
  {
    const int imagesCount = 6;
    assert(paths.size() == imagesCount);
    widths_.resize(imagesCount);
    heights_.resize(imagesCount);
    colorChannelsCounts_.resize(imagesCount);
    if (doOpenGLStuff)
      init();
    else
      loadData();
  }
  Texture(const Texture &) = delete;
  Texture(Texture &&texture) noexcept
      : texture_(texture.texture_),
        paths_(std::move(texture.paths_)),
        widths_(std::move(texture.widths_)),
        heights_(std::move(texture.heights_)),
        colorChannelsCounts_(std::move(texture.colorChannelsCounts_)),
        loadedImages_(std::move(texture.loadedImages_)),
        type_(texture.type_)
  {
    texture.texture_ = 0;
  }
  Texture &operator=(const Texture &) = delete;
  Texture &operator=(Texture &&texture) noexcept
  {
    if (this == &texture)
      return *this;

    release();

    std::swap(texture_, texture.texture_);
    paths_ = std::move(texture.paths_);
    widths_ = std::move(texture.widths_);
    heights_ = std::move(texture.heights_);
    colorChannelsCounts_ = std::move(texture.colorChannelsCounts_);
    loadedImages_ = std::move(texture.loadedImages_);
    type_ = texture.type_;

    return *this;
  }
  ~Texture()
  {
    release();
  }

  void complete()
  {
    assert(!texture_);
    if (type_ == Type::Texture2D)
      createTexture2D();
    else if (type_ == Type::TextureCubeMap)
      createTextureCubeMap();
  }
  void use(uint32_t textureSlot = 0)
  {
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    if (type_ == Type::Texture2D)
      glBindTexture(GL_TEXTURE_2D, texture_);
    else if (type_ == Type::TextureCubeMap)
      glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
  }
  void release()
  {
    if (texture_)
    {
      glDeleteTextures(1, &texture_);
      texture_ = 0;
    }
  }

  const std::string &getPath() const
  {
    return paths_.front();
  }
  const std::vector<std::string> &getPaths() const
  {
    return paths_;
  }
  int getWidth() const
  {
    return widths_.front();
  }
  int getHeight() const
  {
    return heights_.front();
  }
  Type getType() const
  {
    return type_;
  }

 private:
  GLuint texture_ = 0;
  std::vector<std::string> paths_;
  std::vector<int> widths_, heights_, colorChannelsCounts_;
  std::vector<uint8_t *> loadedImages_;
  Type type_;

  void init();
  void loadData();
  void loadTexture2DData();
  void loadTextureCubeMapData();
  uint8_t *loadImageAtPath(int pathsIndex, bool flip = true);
  void createTexture2D();
  void createTextureCubeMap();
};
}  // namespace Simple3D

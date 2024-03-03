#pragma once

#include <glad/glad.h>

#include <assert.h>
#include <filesystem>
#include <simple_3d_viewer/utils/Image.hpp>
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
  explicit Texture(const std::filesystem::path& path, bool doOpenGLStuff = true)
      : type_(Type::Texture2D)
  {
    const int imagesCount = 1;
    paths_.emplace_back(path);
    loadedImages_.reserve(imagesCount);
    if (doOpenGLStuff)
    {
      init();
    }
    else
    {
      loadData();
    }
  }
  explicit Texture(
      std::vector<std::filesystem::path> paths,
      bool doOpenGLStuff = true)
      : paths_(std::move(paths)),
        type_(Type::TextureCubeMap)
  {
    const int imagesCount = 6;
    assert(paths.size() == imagesCount);
    if (doOpenGLStuff)
    {
      init();
    }
    else
    {
      loadData();
    }
  }
  Texture(const Texture&) = delete;
  Texture(Texture&& texture) noexcept
      : textureHandle_(texture.textureHandle_),
        paths_(std::move(texture.paths_)),
        loadedImages_(std::move(texture.loadedImages_)),
        type_(texture.type_)
  {
    texture.textureHandle_ = 0;
  }
  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&& other) noexcept
  {
    if (this == &other)
    {
      return *this;
    }

    release();

    using std::swap;

    swap(textureHandle_, other.textureHandle_);
    swap(paths_, other.paths_);
    swap(loadedImages_, other.loadedImages_);
    swap(type_, other.type_);

    return *this;
  }

  ~Texture()
  {
    release();
  }

  void complete()
  {
    assert(textureHandle_ != 0);

    if (type_ == Type::Texture2D)
    {
      createTexture2D();
    }
    else if (type_ == Type::TextureCubeMap)
    {
      createTextureCubeMap();
    }
  }
  void use(uint32_t textureSlot = 0)
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

  void release()
  {
    if (textureHandle_ == 0)
    {
      return;
    }

    glDeleteTextures(1, &textureHandle_);
    textureHandle_ = 0;
  }

  [[nodiscard]] const std::filesystem::path& getPath() const
  {
    return paths_.front();
  }
  [[nodiscard]] const std::vector<std::filesystem::path>& getPaths() const
  {
    return paths_;
  }

 private:
  GLuint textureHandle_ = 0;
  std::vector<std::filesystem::path> paths_;
  std::vector<Image> loadedImages_;
  Type type_;

  void init();
  void loadData();
  void loadTextures();
  void createTexture2D();
  void createTextureCubeMap();
};
}  // namespace Simple3D

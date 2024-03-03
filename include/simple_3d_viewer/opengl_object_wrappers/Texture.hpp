#pragma once

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
 public:
  enum class Type
  {
    Texture2D,
    TextureCubeMap
  };

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

  void release();

  void complete();

  void use(uint32_t textureSlot = 0);

  [[nodiscard]] const std::filesystem::path& getPath() const
  {
    return paths_.front();
  }
  [[nodiscard]] const std::vector<std::filesystem::path>& getPaths() const
  {
    return paths_;
  }

 private:
  uint textureHandle_ = 0;
  std::vector<std::filesystem::path> paths_;
  std::vector<Image> loadedImages_;
  Type type_;

  void init();
  void loadData();
  void loadTextures();
};

}  // namespace Simple3D

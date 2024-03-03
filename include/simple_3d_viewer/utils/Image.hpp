#pragma once

#include <cstdint>
#include <simple_3d_viewer/utils/Size.hpp>
#include <utility>

namespace Simple3D
{

struct Image
{
  Image(uint8_t* pImage, Size pSize, int pColorChannelCount);
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
  Image(Image&& other) noexcept;
  Image& operator=(Image&& other) noexcept;
  ~Image();

  void release();

  friend void swap(Image& lhs, Image& rhs)
  {
    using std::swap;

    swap(lhs.image, rhs.image);
    swap(lhs.size, rhs.size);
    swap(lhs.colorChannelCount, rhs.colorChannelCount);
  }

  uint8_t* image = nullptr;
  Size size{};
  int colorChannelCount{};
};

}  // namespace Simple3D
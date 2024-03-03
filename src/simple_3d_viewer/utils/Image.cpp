#include <cstddef>
#include <simple_3d_viewer/utils/Image.hpp>
#include <stb_image.h>
#include <utility>

namespace Simple3D
{

Image::Image(uint8_t* pImage, Size pSize, int pColorChannelCount)
    : image(pImage),
      size(pSize),
      colorChannelCount(pColorChannelCount)
{
}

Image::Image(Image&& other) noexcept
    : image(other.image),
      size(other.size),
      colorChannelCount(other.colorChannelCount)
{
  other.image = nullptr;
}

void Image::release()
{
  if (image == nullptr)
  {
    return;
  }

  stbi_image_free(image);
  image = nullptr;
}

Image& Image::operator=(Image&& other) noexcept
{
  if (this == &other)
  {
    return *this;
  }

  release();

  using std::swap;

  swap(image, other.image);
  swap(size, other.size);
  swap(colorChannelCount, other.colorChannelCount);

  return *this;
}

Image::~Image()
{
  release();
}

}  // namespace Simple3D
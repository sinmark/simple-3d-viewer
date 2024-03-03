#pragma once

namespace Simple3D
{

struct Size
{
  int width;
  int height;

  friend bool operator==(Size lhs, Size rhs)
  {
    return lhs.width == rhs.width && lhs.height == rhs.height;
  }
};

}  // namespace Simple3D

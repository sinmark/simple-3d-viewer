#pragma once

namespace Simple3D
{

struct Size
{
  int width;
  int height;

  friend bool operator==(Size lhs, Size rhs) = default;
};

}  // namespace Simple3D

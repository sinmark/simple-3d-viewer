#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

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

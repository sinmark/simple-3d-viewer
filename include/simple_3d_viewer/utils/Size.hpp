#pragma once

#include <GLFW/glfw3.h>

namespace Simple3D {
struct Size {
  int width;
  int height;

  friend bool operator==(Size lhs, Size rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
  }
};

inline Size getFramebufferSize(GLFWwindow *window) {
  int width{};
  int height{};
  glfwGetFramebufferSize(window, &width, &height);

  return {width, height};
}
} // namespace Simple3D

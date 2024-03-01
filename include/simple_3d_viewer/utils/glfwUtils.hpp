#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <simple_3d_viewer/utils/Size.hpp>

namespace Simple3D
{

inline Size getFramebufferSize(GLFWwindow* window)
{
  int width{};
  int height{};
  glfwGetFramebufferSize(window, &width, &height);

  return { width, height };
}

}  // namespace Simple3D

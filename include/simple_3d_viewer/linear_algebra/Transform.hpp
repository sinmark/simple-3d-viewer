#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <simple_3d_viewer/utils/Size.hpp>

namespace Simple3D {
struct Transform {
  glm::vec3 translation;
  glm::vec3 rotation;
  glm::vec3 scale;
};

glm::mat4x4 calculateModelTransform(const Transform& transform);
glm::mat4x4 calculateProjectionTransform(Size size);
} // namespace Simple3D

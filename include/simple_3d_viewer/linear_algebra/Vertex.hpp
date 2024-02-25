#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Simple3D
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static constexpr size_t maxUVChannels = 8;
    std::array<glm::vec2, maxUVChannels> texCoords;
  };
  static_assert(sizeof(glm::vec3) == 3 * sizeof(float));
  static_assert(
      sizeof(std::array<glm::vec2, Vertex::maxUVChannels>) ==
      sizeof(glm::vec2) * Vertex::maxUVChannels);
}  // namespace Simple3D

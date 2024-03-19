#include <cstddef>
#include <cstdint>
#include <glm/vec3.hpp>
#include <numbers>
#include <simple_3d_viewer/linear_algebra/Vertex.hpp>
#include <simple_3d_viewer/linear_algebra/meshGeneration.hpp>
#include <vector>

namespace Simple3D
{

// https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
Mesh sphereMesh(float radius)
{
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  static constexpr auto pi = std::numbers::pi_v<float>;

  static constexpr uint32_t latitudes = 30;
  static constexpr uint32_t longitudes = 30;
  static_assert(latitudes >= 2 && longitudes >= 3);

  vertices.reserve(
      static_cast<std::vector<Vertex>::size_type>(latitudes) * longitudes);
  indices.reserve((latitudes - 2) * longitudes * 6 + 2 * longitudes * 3);
  const auto lengthInv = 1.f / radius;
  const auto deltaLatitude = pi / latitudes;
  const auto deltaLongitude = (2 * pi) / longitudes;

  for (uint32_t i = 0; i <= latitudes; ++i)
  {
    const auto latitudeAngle =
        pi / 2 -
        static_cast<float>(i) * deltaLatitude; /* Starting -pi/2 to pi/2 */
    const auto forXYComponent =
        radius * std::cos(latitudeAngle); /* r * cos(phi) */
    const auto forZComponent =
        radius * std::sin(latitudeAngle); /* r * sin(phi )*/

    /*
     * We add (latitudes + 1) vertices per longitude because of equator,
     * the North pole and South pole are not counted here, as they overlap.
     * The first and last vertices have same position and normal, but
     * different tex coords.
     */
    for (size_t j = 0; j <= longitudes; ++j)
    {
      const auto longitudeAngle = static_cast<float>(j) * deltaLongitude;

      Vertex vertex{};
      vertex.position.x =
          forXYComponent *
          std::cos(longitudeAngle); /* x = r * cos(phi) * cos(theta)  */
      vertex.position.y =
          forXYComponent *
          std::sin(longitudeAngle);      /* y = r * cos(phi) * sin(theta) */
      vertex.position.z = forZComponent; /* z = r * sin(phi) */
      vertex.texCoords[0].s = static_cast<float>(j) / longitudes; /* s */
      vertex.texCoords[0].t = static_cast<float>(i) / latitudes;  /* t */
      vertex.normal.x = vertex.position.x * lengthInv;
      vertex.normal.y = vertex.position.y * lengthInv;
      vertex.normal.z = vertex.position.z * lengthInv;
      vertices.push_back(vertex);
    }
  }

  /*
   *  Indices
   *  k1--k1+1
   *  |  / |
   *  | /  |
   *  k2--k2+1
   */
  for (uint32_t i = 0; i < latitudes; ++i)
  {
    auto k1 = i * (longitudes + 1);
    auto k2 = k1 + longitudes + 1;
    // 2 Triangles per latitude block excluding the first and last longitudes
    // blocks
    for (uint32_t j = 0; j < longitudes; ++j, ++k1, ++k2)
    {
      if (i != 0)
      {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if (i != (latitudes - 1))
      {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }

  return { std::move(vertices), std::move(indices) };
}

Mesh skyboxMesh()
{
  std::vector<Vertex> squareVertices = {
    { glm::vec3(-1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, 1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, 1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, -1.0f), {}, {}, {} },
    { glm::vec3(-1.0f, -1.0f, 1.0f), {}, {}, {} },
    { glm::vec3(1.0f, -1.0f, 1.0f), {}, {}, {} }
  };
  return Mesh{ std::move(squareVertices) };
}

}  // namespace Simple3D

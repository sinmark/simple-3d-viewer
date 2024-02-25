#include <glm/gtx/rotate_vector.hpp>
#include <simple_3d_viewer/linear_algebra/Transform.hpp>

namespace Simple3D
{
glm::mat4x4 calculateModelTransform(const Transform& transform)
{
  const auto& [translation, rotation, scale] = transform;

  const glm::vec3 xRotationAxis(1.0f, 0.0f, 0.0f);
  const glm::vec3 yRotationAxis(0.0f, 1.0f, 0.0f);
  const glm::vec3 zRotationAxis(0.0f, 0.0f, 1.0f);

  const glm::mat4x4 identity(1.0f);
  const auto scaleMat = glm::scale(identity, scale);
  const auto rotateXMat =
      glm::rotate(identity, glm::radians(rotation.x), xRotationAxis);
  const auto rotateYMat =
      glm::rotate(identity, glm::radians(rotation.y), yRotationAxis);
  const auto rotateZMat =
      glm::rotate(identity, glm::radians(rotation.z), zRotationAxis);
  const auto translateMat = glm::translate(identity, translation);

  return translateMat * rotateZMat * rotateYMat * rotateXMat * scaleMat;
}

glm::mat4x4 calculateProjectionTransform(Size size)
{
  static constexpr auto fovDeg = 45.0f;
  static constexpr auto nearPlane = 0.1f;
  static constexpr auto farPlane = 100.0f;
  return glm::perspective(
      glm::radians(fovDeg),
      static_cast<float>(size.width) / static_cast<float>(size.height),
      nearPlane,
      farPlane);
}
}  // namespace Simple3D

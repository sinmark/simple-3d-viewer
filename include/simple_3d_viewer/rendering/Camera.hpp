#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <simple_3d_viewer/utils/Size.hpp>

namespace Simple3D
{
class Camera
{
 public:
  struct Settings
  {
    float speed;
    float sensitivity;
  };

  Camera() = delete;
  Camera(GLFWwindow* window) : window_(window)
  {
    auto [width, height] = getFramebufferSize(window_);
    lastMouseX_ = width / 2.0;
    lastMouseY_ = height / 2.0;
  }
  Camera(const Camera&) = delete;
  Camera(Camera&&) = delete;

  const glm::vec3& getPosition() const
  {
    return position_;
  }
  glm::mat4 getViewTransform() const
  {
    return glm::lookAt(position_, position_ + orientation_, up_);
  }
  void setSettings(Settings settings)
  {
    settings_ = settings;
  }

  void processInput(float delta);

 private:
  GLFWwindow* window_{ nullptr };
  glm::vec3 position_{ 0.f, 0.f, 5.f };
  glm::vec3 orientation_{ 0.f, 0.f, -1.f };
  glm::vec3 up_{ 0.f, 1.f, 0.f };
  Settings settings_{ 1.f, 0.1f };
  double yaw_ = -90., pitch_ = 0.;
  double lastMouseX_, lastMouseY_;
  bool firstClick_ = true;
};
}  // namespace Simple3D

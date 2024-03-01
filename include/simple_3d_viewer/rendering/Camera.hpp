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
  explicit Camera(Size frameBufferSize)
      : lastMouseX_(static_cast<float>(frameBufferSize.width) / 2.f),
        lastMouseY_(static_cast<float>(frameBufferSize.height) / 2.f)
  {
  }
  // The class has state which we would need to reset for the class to work
  // properly. Also for our use case there is no need for more than one Camera
  // class.
  Camera(const Camera&) = delete;
  Camera& operator=(const Camera&) = delete;
  Camera(Camera&&) = delete;
  Camera& operator=(Camera&&) = delete;
  ~Camera() = default;

  [[nodiscard]] const glm::vec3& getPosition() const
  {
    return position_;
  }
  [[nodiscard]] glm::mat4 getViewTransform() const
  {
    return glm::lookAt(position_, position_ + orientation_, up_);
  }
  void setSettings(Settings settings)
  {
    settings_ = settings;
  }

  void processInput(float delta, GLFWwindow* window);

 private:
  glm::vec3 position_{ 0.f, 0.f, 5.f };
  glm::vec3 orientation_{ 0.f, 0.f, -1.f };
  glm::vec3 up_{ 0.f, 1.f, 0.f };
  Settings settings_{ 1.f, 0.1f };
  float yaw_ = -90.f;
  float pitch_ = 0.f;
  float lastMouseX_;
  float lastMouseY_;
  bool firstClick_ = true;
};

}  // namespace Simple3D

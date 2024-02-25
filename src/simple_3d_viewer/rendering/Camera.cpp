#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <simple_3d_viewer/rendering/Camera.hpp>

namespace Simple3D
{
void Camera::processInput(float delta)
{
  const auto [speed, sensitivity] = settings_;
  const float deltaSpeed = speed * delta;
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
  {
    position_ += deltaSpeed * orientation_;
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
  {
    position_ += deltaSpeed * -glm::normalize(glm::cross(orientation_, up_));
  }
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
  {
    position_ += deltaSpeed * -orientation_;
  }
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
  {
    position_ += deltaSpeed * glm::normalize(glm::cross(orientation_, up_));
  }
  if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    position_ += deltaSpeed * up_;
  }
  if (glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
  {
    position_ += deltaSpeed * -up_;
  }

  const float deltaSensitivity = sensitivity * delta;
  const auto rightMouseButtonClickState =
      glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT);
  if (rightMouseButtonClickState == GLFW_PRESS)
  {
    double mouseX{};
    double mouseY{};
    glfwGetCursorPos(window_, &mouseX, &mouseY);

    if (firstClick_)
    {
      lastMouseX_ = mouseX;
      lastMouseY_ = mouseY;
      firstClick_ = false;
    }

    double xoffset = mouseX - lastMouseX_;
    double yoffset =
        lastMouseY_ -
        mouseY;  // reversed since y-coordinates range from bottom to top
    lastMouseX_ = mouseX;
    lastMouseY_ = mouseY;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw_ = glm::mod(yaw_ + xoffset, 360.0);
    pitch_ += yoffset;

    if (pitch_ > 89.0f)
      pitch_ = 89.0f;
    if (pitch_ < -89.0f)
      pitch_ = -89.0f;

    const glm::vec3 direction{
      static_cast<float>(cos(glm::radians(yaw_)) * cos(glm::radians(pitch_))),
      static_cast<float>(sin(glm::radians(pitch_))),
      static_cast<float>(sin(glm::radians(yaw_)) * cos(glm::radians(pitch_)))
    };
    orientation_ = glm::normalize(direction);
  }
  else if (rightMouseButtonClickState == GLFW_RELEASE)
  {
    firstClick_ = true;
  }
}
}  // namespace Simple3D

#include <algorithm>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <simple_3d_viewer/rendering/Camera.hpp>

namespace Simple3D
{

void Camera::processInput(float delta, GLFWwindow* window)
{
  const auto [speed, sensitivity] = settings_;
  const float deltaSpeed = speed * delta;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    position_ += deltaSpeed * orientation_;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    position_ += deltaSpeed * -glm::normalize(glm::cross(orientation_, up_));
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    position_ += deltaSpeed * -orientation_;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    position_ += deltaSpeed * glm::normalize(glm::cross(orientation_, up_));
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    position_ += deltaSpeed * up_;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
  {
    position_ += deltaSpeed * -up_;
  }

  const auto deltaSensitivity = sensitivity * delta;
  const auto rightMouseButtonClickState =
      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
  if (rightMouseButtonClickState == GLFW_PRESS)
  {
    double mouseXTemp{};
    double mouseYTemp{};
    glfwGetCursorPos(window, &mouseXTemp, &mouseYTemp);

    const auto mouseX = static_cast<float>(mouseXTemp);
    const auto mouseY = static_cast<float>(mouseYTemp);

    if (firstClick_)
    {
      lastMouseX_ = mouseX;
      lastMouseY_ = mouseY;
      firstClick_ = false;
    }

    auto xoffset = mouseX - lastMouseX_;
    auto yoffset =
        lastMouseY_ -
        mouseY;  // reversed since y-coordinates range from bottom to top
    lastMouseX_ = mouseX;
    lastMouseY_ = mouseY;

    xoffset *= deltaSensitivity;
    yoffset *= deltaSensitivity;

    yaw_ = glm::mod(yaw_ + xoffset, 360.0f);
    pitch_ += yoffset;

    constexpr auto pitchBound = 89.0f;
    std::clamp(pitch_, -pitchBound, pitchBound);

    const glm::vec3 direction{
      std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_)),
      std::sin(glm::radians(pitch_)),
      std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_))
    };
    orientation_ = glm::normalize(direction);
  }
  else if (rightMouseButtonClickState == GLFW_RELEASE)
  {
    firstClick_ = true;
  }
}

}  // namespace Simple3D

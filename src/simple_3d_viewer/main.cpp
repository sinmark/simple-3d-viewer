#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <filesystem>
#include <simple_3d_viewer/ImGuiWrapper.hpp>
#include <simple_3d_viewer/Mediator.hpp>
#include <simple_3d_viewer/Viewer.hpp>
#include <simple_3d_viewer/utils/glfwUtils.hpp>

int main()
{
  // GLFW init stuff
  if (!glfwInit())
    return -1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  const int initialScreenWidth = 800;
  const int initialScreenHeight = 600;
  GLFWwindow* window = nullptr;
  window = glfwCreateWindow(
      initialScreenWidth,
      initialScreenHeight,
      "Simple 3D Viewer",
      nullptr,
      nullptr);
  if (window == nullptr)
  {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
  {
    glfwTerminate();
    return -1;
  }
  glfwSwapInterval(1);

  std::vector<std::string> supportedPostprocesses = { "FXAA",
                                                      "inversion",
                                                      "grayscale" };
  Simple3D::ImGuiWrapper imGuiWrapper(window, supportedPostprocesses);
  Simple3D::Viewer viewer(window, supportedPostprocesses);
  Simple3D::Mediator mediator(imGuiWrapper, viewer);
  imGuiWrapper.sync();

  double previousTime = 0;
  double currentTime = glfwGetTime();
  while (!glfwWindowShouldClose(window))
  {
    previousTime = currentTime;
    currentTime = glfwGetTime();
    const double delta = currentTime - previousTime;

    imGuiWrapper.update();
    viewer.processInput(static_cast<float>(delta));

    const auto framebufferSize = Simple3D::getFramebufferSize(window);
    viewer.render(framebufferSize);
    imGuiWrapper.render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
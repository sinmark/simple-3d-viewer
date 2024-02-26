#pragma once

#include <optional>
#include <simple_3d_viewer/linear_algebra/meshGeneration.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/rendering/Camera.hpp>
#include <simple_3d_viewer/rendering/Mesh.hpp>
#include <simple_3d_viewer/rendering/Model.hpp>

namespace Simple3D
{
inline const std::string lightProgramFilename = "light";
inline const std::string skyboxProgramFilename = "skybox";
inline const std::vector<std::string> skyboxImagesPaths = {
  "res/textures/skybox/right.jpg", "res/textures/skybox/left.jpg",
  "res/textures/skybox/top.jpg",   "res/textures/skybox/bottom.jpg",
  "res/textures/skybox/front.jpg", "res/textures/skybox/back.jpg"
};
inline const std::string modelProgramFilename = "model";

struct Scene
{
  Scene(GLFWwindow* window)
      : camera(window),
        light(sphereMesh(0.1f)),
        lightProgram(lightProgramFilename),
        skybox(skyboxMesh()),
        skyboxProgram(skyboxProgramFilename),
        skyboxTexture(skyboxImagesPaths),
        modelProgram(modelProgramFilename)
  {
  }

  Camera camera;
  Mesh light;
  Program lightProgram;
  glm::vec3 lightPosition{};
  Mesh skybox;
  Program skyboxProgram;
  Texture skyboxTexture;
  std::optional<Model> model;
  Program modelProgram;
};
}  // namespace Simple3D

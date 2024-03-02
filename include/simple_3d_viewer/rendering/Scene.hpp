#pragma once

#include <optional>
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/rendering/Camera.hpp>
#include <simple_3d_viewer/rendering/Mesh.hpp>
#include <simple_3d_viewer/rendering/Model.hpp>
#include <simple_3d_viewer/utils/Size.hpp>

namespace Simple3D
{

struct Scene
{
  explicit Scene(Size framebufferSize);

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

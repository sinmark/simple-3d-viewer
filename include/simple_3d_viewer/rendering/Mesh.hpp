#pragma once

#include <array>
#include <cassert>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <simple_3d_viewer/linear_algebra/Vertex.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/rendering/Camera.hpp>
#include <simple_3d_viewer/rendering/Material.hpp>
#include <string>
#include <vector>

namespace Simple3D
{

class Mesh
{
 public:
  Mesh() = delete;
  explicit Mesh(
      std::vector<Vertex> vertices,
      bool issueRenderingAPICalls = true)
      : vertices_(std::move(vertices))
  {
    if (issueRenderingAPICalls)
    {
      init();
    }
  }
  Mesh(
      std::vector<Vertex> vertices,
      std::vector<GLuint> indices,
      bool issueRenderingAPICalls = true)
      : vertices_(std::move(vertices)),
        indices_(std::move(indices))
  {
    if (issueRenderingAPICalls)
    {
      init();
    }
  }
  Mesh(
      std::vector<Vertex> vertices,
      std::vector<GLuint> indices,
      Material* material,
      bool issueRenderingAPICalls = true)
      : vertices_(std::move(vertices)),
        indices_(std::move(indices)),
        material_(material)
  {
    if (issueRenderingAPICalls)
    {
      init();
    }
  }
  Mesh(const Mesh& mesh) = delete;
  Mesh& operator=(const Mesh&) = delete;
  Mesh(Mesh&& mesh) noexcept
      : vertices_(std::move(mesh.vertices_)),
        indices_(std::move(mesh.indices_)),
        material_(mesh.material_),
        vao_(mesh.vao_),
        vbo_(mesh.vbo_),
        ebo_(mesh.ebo_)
  {
    mesh.vao_ = 0;
    mesh.vbo_ = 0;
    mesh.ebo_ = 0;
    mesh.material_ = nullptr;
  }
  Mesh& operator=(Mesh&& other) noexcept
  {
    if (this == &other)
    {
      return *this;
    }

    release();

    using std::swap;

    swap(vao_, other.vao_);
    swap(vbo_, other.vbo_);
    swap(ebo_, other.ebo_);
    swap(vertices_, other.vertices_);
    swap(indices_, other.indices_);
    swap(material_, other.material_);

    return *this;
  }
  ~Mesh()
  {
    release();
  }

  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
  Material* material_{ nullptr };
  uint vao_{};
  uint vbo_{};
  uint ebo_{};

  void complete();

  void release();

 private:
  void init();
};

}  // namespace Simple3D
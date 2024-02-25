#pragma once

#include <assert.h>
#include <glad/glad.h>

#include <array>
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
  Mesh(std::vector<Vertex> &&vertices, bool issueRenderingAPICalls = true)
      : vertices_(std::move(vertices))
  {
    if (issueRenderingAPICalls)
      init();
  }
  Mesh(
      std::vector<Vertex> &&vertices,
      std::vector<GLuint> &&indices,
      bool issueRenderingAPICalls = true)
      : vertices_(std::move(vertices)),
        indices_(std::move(indices))
  {
    if (issueRenderingAPICalls)
      init();
  }
  Mesh(
      std::vector<Vertex> &&vertices,
      std::vector<GLuint> &&indices,
      Material *material,
      bool issueRenderingAPICalls = true)
      : vertices_(std::move(vertices)),
        indices_(std::move(indices)),
        material_(material)
  {
    if (issueRenderingAPICalls)
      init();
  }
  Mesh(const Mesh &mesh) = delete;
  Mesh(Mesh &&mesh) noexcept
      : vao_(mesh.vao_),
        vbo_(mesh.vbo_),
        ebo_(mesh.ebo_),
        vertices_(std::move(mesh.vertices_)),
        indices_(std::move(mesh.indices_)),
        material_(mesh.material_)
  {
    mesh.vao_ = 0;
    mesh.vbo_ = 0;
    mesh.ebo_ = 0;
    mesh.material_ = nullptr;
  }
  Mesh &operator=(const Mesh &) = delete;
  Mesh &operator=(Mesh &&mesh) noexcept
  {
    if (this == &mesh)
      return *this;

    release();

    std::swap(vao_, mesh.vao_);
    std::swap(vbo_, mesh.vbo_);
    std::swap(ebo_, mesh.ebo_);
    vertices_ = std::move(mesh.vertices_);
    indices_ = std::move(mesh.indices_);
    material_ = mesh.material_;

    return *this;
  }
  ~Mesh()
  {
    release();
  }

  GLuint vao_{ 0 }, vbo_{ 0 }, ebo_{ 0 };
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
  Material *material_{ nullptr };

  void complete()
  {
    if (vao_)
      throw std::logic_error("Object is already complete!");
    init();
  }
  void release()
  {
    if (vao_)
    {
      assert(vao_ && vbo_);
      glDeleteVertexArrays(1, &vao_);
      glDeleteBuffers(1, &vbo_);
      if (ebo_)
      {
        glDeleteBuffers(1, &ebo_);
      }
      vao_ = 0;
      vbo_ = 0;
      ebo_ = 0;
    }
  }

 private:
  void init();
};
}  // namespace Simple3D
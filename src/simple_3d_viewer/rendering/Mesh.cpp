#include <glad/glad.h>

#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <simple_3d_viewer/rendering/Mesh.hpp>
#include <simple_3d_viewer/utils/constants.hpp>

namespace Simple3D
{

namespace
{

char* attributeOffset(GLuint value)
{
  return std::next(static_cast<char*>(nullptr), value);
}

}  // namespace

void Mesh::init()
{
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizeiptr>(vertices_.size() * sizeof(Vertex)),
      vertices_.data(),
      GL_STATIC_DRAW);

  GLuint index = 0;
  GLuint offset = 0;
  glVertexAttribPointer(
      index,
      kVec3ComponentsCount,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      attributeOffset(offset));
  glEnableVertexAttribArray(index);

  ++index;
  offset += sizeof(glm::vec3);
  glVertexAttribPointer(
      index,
      kVec3ComponentsCount,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      attributeOffset(offset));
  glEnableVertexAttribArray(index);

  ++index;
  offset += sizeof(glm::vec3);
  glVertexAttribPointer(
      index,
      kVec3ComponentsCount,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      attributeOffset(offset));
  glEnableVertexAttribArray(index);

  ++index;
  offset += sizeof(glm::vec3);
  glVertexAttribPointer(
      index,
      kVec3ComponentsCount,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      attributeOffset(offset));
  glEnableVertexAttribArray(index);

  ++index;
  offset += sizeof(glm::vec2);
  glVertexAttribPointer(
      index,
      kVec2ComponentsCount,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      attributeOffset(offset));
  glEnableVertexAttribArray(index);

  ++index;
  offset += sizeof(glm::vec2);
  glVertexAttribPointer(
      index,
      kVec2ComponentsCount,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      attributeOffset(offset));
  glEnableVertexAttribArray(index);

  ++index;
  offset += sizeof(glm::vec2);
  glVertexAttribPointer(
      index,
      kVec2ComponentsCount,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      attributeOffset(offset));
  glEnableVertexAttribArray(index);

  ++index;
  offset += sizeof(glm::vec2);
  glVertexAttribPointer(
      index,
      kVec2ComponentsCount,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      attributeOffset(offset));
  glEnableVertexAttribArray(index);

  if (!indices_.empty())
  {
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices_.size() * sizeof(GLuint)),
        indices_.data(),
        GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::release()
{
  if (vao_ != 0)
  {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    if (ebo_ != 0)
    {
      glDeleteBuffers(1, &ebo_);
    }
    vao_ = 0;
    vbo_ = 0;
    ebo_ = 0;
  }
}

void Mesh::complete()
{
  if (vao_ != 0)
  {
    throw std::logic_error("Object is already complete!");
  }
  init();
}

}  // namespace Simple3D

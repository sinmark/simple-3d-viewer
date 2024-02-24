#include <simple_3d_viewer/rendering/Mesh.hpp>

namespace Simple3D {
void Mesh::init() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
               vertices_.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(9 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(11 * sizeof(float)));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(13 * sizeof(float)));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(15 * sizeof(float)));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(17 * sizeof(float)));
  glEnableVertexAttribArray(7);

  if (!indices_.empty()) {
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint),
                 indices_.data(), GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
} // namespace Simple3D

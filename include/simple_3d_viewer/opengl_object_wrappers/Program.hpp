#pragma once

#include <glad/glad.h>

#include <functional>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Simple3D
{

class Program
{
 public:
  Program() = delete;
  Program(const std::string& vertexShader, const std::string& fragmentShader);
  Program(const Program& other) = delete;
  Program(Program&& other) noexcept;
  Program& operator=(const Program&) = delete;
  Program& operator=(Program&& other) noexcept;
  ~Program();

  void doOperations(const std::function<void(Program&)>& operations);
  void release();

  [[nodiscard]] uint64_t getID() const
  {
    return id_;
  }

  [[nodiscard]] bool hasUniform(const std::string& name) const;

  void setInt(const std::string& name, int value);
  void setFloat(const std::string& name, float value);
  void setVec2f(const std::string& name, const glm::vec2& vector);
  void setVec2f(const std::string& name, float v0, float v1);
  void setVec3f(const std::string& name, const glm::vec3& vector);
  void setVec3f(const std::string& name, float v0, float v1, float v2);
  void setVec4f(const std::string& name, const glm::vec4& vector);
  void
  setVec4f(const std::string& name, float v0, float v1, float v2, float v3);
  void setMat4f(const std::string& name, const glm::mat4& matrix);

 private:
  GLuint programHandle_ = 0;
  uint64_t id_;
  std::unordered_map<std::string, GLint> uniformLocationCache_;
};

}  // namespace Simple3D

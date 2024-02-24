#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Simple3D {
class Program {
public:
  Program() = delete;
  Program(const std::string &vertexShaderFileName,
          const std::string &fragmentShaderFileName)
      : program_(glCreateProgram()), id_(idGenerator_++) {
    loadShaders(vertexShaderFileName + ".vs", fragmentShaderFileName + ".fs");
    linkProgram();
  }
  Program(const std::string &commonShaderFileName)
      : program_(glCreateProgram()), id_(idGenerator_++) {
    loadShaders(commonShaderFileName + ".vs", commonShaderFileName + ".fs");
    linkProgram();
  }
  Program(const Program &program) = delete;
  Program(Program &&program) noexcept
      : program_(program.program_),
        uniformLocationCache_(std::move(program.uniformLocationCache_)),
        beingUsed_(program.beingUsed_), usedCount_(program.usedCount_),
        vertexShaderCode_(std::move(program.vertexShaderCode_)),
        fragmentShaderCode_(std::move(program.fragmentShaderCode_)),
        id_(program.id_) {
    program.program_ = 0;
  }
  Program &operator=(const Program &) = delete;
  Program &operator=(Program &&program) noexcept {
    if (this == &program)
      return *this;

    release();

    std::swap(program_, program.program_);
    uniformLocationCache_ = std::move(program.uniformLocationCache_);
    beingUsed_ = program.beingUsed_;
    usedCount_ = program.usedCount_;
    vertexShaderCode_ = std::move(program.vertexShaderCode_);
    fragmentShaderCode_ = std::move(program.fragmentShaderCode_);
    id_ = program.id_;

    return *this;
  }
  ~Program() { release(); }

  void doOperations(std::function<void(Program &)> operations) {
    use();
    operations(*this);
    stopUsing();
  }
  void release() {
    if (program_ != 0)
      glDeleteProgram(program_);
    program_ = 0;
  }
  uint64_t getID() const { return id_; }

  void setInt(const std::string &name, int value);
  void setFloat(const std::string &name, float value);
  void setVec2f(const std::string &name, const glm::vec2 &vector);
  void setVec2f(const std::string &name, float v0, float v1);
  void setVec3f(const std::string &name, const glm::vec3 &vector);
  void setVec3f(const std::string &name, float v0, float v1, float v2);
  void setVec4f(const std::string &name, const glm::vec4 &vector);
  void setVec4f(const std::string &name, float v0, float v1, float v2,
                float v3);
  void setMat4f(const std::string &name, const glm::mat4 &matrix);

  bool hasUniform(const std::string &name) const;

private:
  GLuint program_ = 0;
  std::unordered_map<std::string, GLint> uniformLocationCache_;
  std::unordered_set<std::string> invalidUniformsCache_;
  bool beingUsed_ = false;
  uint32_t usedCount_ = 0;
  uint64_t id_;
  // For debugging
  std::string vertexShaderCode_;
  std::string fragmentShaderCode_;

  static uint64_t idGenerator_;

  void use() {
    if (usedCount_ == 0) {
      glUseProgram(program_);
      beingUsed_ = true;
    }
    assert(beingUsed_);
    ++usedCount_;
  }
  void stopUsing() {
    if (usedCount_ == 1) {
      assert(beingUsed_);
      glUseProgram(0);
      beingUsed_ = false;
    }
    --usedCount_;
  }

  void loadShaders(const std::string &vertexShaderFileName,
                   const std::string &fragmentShaderFileName);
  void linkProgram();
  GLint getUniformLocation(const std::string &name);
};
} // namespace Simple3D

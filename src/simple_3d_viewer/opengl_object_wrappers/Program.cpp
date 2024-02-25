#include <filesystem>
#include <iostream>
#include <optional>
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/utils/fileOperations.hpp>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Simple3D
{
namespace
{
uint64_t g_idGenerator = 0;
constexpr auto errorPrefix = "Program error: ";
constexpr GLint invalidLocation = -1;

// The function call filesystem::current_path() can throw so we can't make
// shaderDirPath global and initialize it at the same time.
std::filesystem::path &shaderDirPath()
{
  static auto result = std::filesystem::current_path() / "res/shaders/";
  return result;
}

std::optional<std::string> getCompilationErrorMessage(GLuint shader)
{
  GLint compiled{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled == 0)
  {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes the NULL character
    std::string errorMessage(maxLength, '\0');
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorMessage.front());
    std::cout << errorPrefix << errorMessage << '\n';
    return errorMessage;
  }
  return {};
}

void printInvalidUniform(const std::string &name)
{
  const std::string log = std::string(errorPrefix) + "Uniform '" + name +
                          "' doesn't exist! Might have been optimized out!";
  std::cout << log << '\n';
}

GLuint linkProgram(
    const std::string &vertexShader,
    const std::string &fragmentShader)
{
  const GLuint programHandle = glCreateProgram();

  const auto vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
  const auto *vertexShaderPtr = vertexShader.c_str();
  glShaderSource(vertexShaderHandle, 1, &vertexShaderPtr, nullptr);
  glCompileShader(vertexShaderHandle);
  if (const auto maybeErrorMessage =
          getCompilationErrorMessage(vertexShaderHandle);
      maybeErrorMessage.has_value())
  {
    glDeleteProgram(programHandle);
    glDeleteShader(vertexShaderHandle);
    throw std::invalid_argument(*maybeErrorMessage);
  }
  glAttachShader(programHandle, vertexShaderHandle);

  const auto fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
  const auto *fragmentShaderPtr = fragmentShader.c_str();
  glShaderSource(fragmentShaderHandle, 1, &fragmentShaderPtr, nullptr);
  glCompileShader(fragmentShaderHandle);
  if (const auto maybeErrorMessage =
          getCompilationErrorMessage(fragmentShaderHandle);
      maybeErrorMessage.has_value())
  {
    glDeleteProgram(programHandle);
    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);
    throw std::invalid_argument(*maybeErrorMessage);
  }
  glAttachShader(programHandle, fragmentShaderHandle);

  glLinkProgram(programHandle);
  GLint linked = 0;
  glGetProgramiv(programHandle, GL_LINK_STATUS, (int *)&linked);
  if (linked == 0)
  {
    GLint maxLength = 0;
    glGetShaderiv(fragmentShaderHandle, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes the NULL character
    std::string errorMessage(maxLength, '\0');
    glGetShaderInfoLog(
        fragmentShaderHandle, maxLength, &maxLength, &errorMessage.front());
    std::cout << errorPrefix << errorMessage << '\n';

    glDeleteProgram(programHandle);
    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);
    throw std::invalid_argument(errorMessage);
  }

  glDetachShader(programHandle, vertexShaderHandle);
  glDetachShader(programHandle, fragmentShaderHandle);
  glDeleteShader(vertexShaderHandle);
  glDeleteShader(fragmentShaderHandle);

  return programHandle;
}
}  // namespace

Program::Program(
    const std::string &vertexShaderFileName,
    const std::string &fragmentShaderFileName)
    : programHandle_(linkProgram(
          loadFileIntoString(shaderDirPath() / (vertexShaderFileName + ".vs")),
          loadFileIntoString(
              shaderDirPath() / (fragmentShaderFileName + ".fs")))),
      id_(++g_idGenerator)
{
}

Program::Program(const std::string &commonShaderFileName)
    : Program(commonShaderFileName, commonShaderFileName)
{
}

Program &Program::operator=(Program &&other) noexcept
{
  if (this == &other)
  {
    return *this;
  }

  release();

  std::swap(programHandle_, other.programHandle_);
  uniformLocationCache_ = std::move(other.uniformLocationCache_);
  id_ = other.id_;

  return *this;
}

Program::Program(Program &&other) noexcept
    : programHandle_(other.programHandle_),
      id_(other.id_),
      uniformLocationCache_(std::move(other.uniformLocationCache_))
{
  other.programHandle_ = 0;
}

Program::~Program()
{
  release();
}

void Program::release()
{
  if (programHandle_ != 0)
  {
    glDeleteProgram(programHandle_);
  }
  programHandle_ = 0;
}

void Program::doOperations(const std::function<void(Program &)> &operations)
{
  glUseProgram(programHandle_);
  operations(*this);
  glUseProgram(0);
}

namespace
{
GLint getUniformLocation(
    const std::string &name,
    GLuint programHandle,
    std::unordered_map<std::string, GLint> &uniformLocationCache)
{
  if (uniformLocationCache.contains(name))
  {
    return uniformLocationCache[name];
  }

  GLint location = glGetUniformLocation(programHandle, name.c_str());
  if (location == invalidLocation)
  {
    printInvalidUniform(name);
  }

  uniformLocationCache.emplace(name, location);
  return location;
}
}  // namespace

void Program::setInt(const std::string &name, int value)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniform1i(location, value);
}

void Program::setFloat(const std::string &name, float value)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniform1f(location, value);
}

void Program::setVec2f(const std::string &name, const glm::vec2 &vector)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniform2f(location, vector[0], vector[1]);
}

void Program::setVec2f(const std::string &name, float v0, float v1)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniform2f(location, v0, v1);
}

void Program::setVec3f(const std::string &name, const glm::vec3 &vector)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniform3f(location, vector[0], vector[1], vector[2]);
}

void Program::setVec3f(const std::string &name, float v0, float v1, float v2)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniform3f(location, v0, v1, v2);
}

void Program::setVec4f(const std::string &name, const glm::vec4 &vector)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniform4f(location, vector[0], vector[1], vector[2], vector[3]);
}

void Program::setVec4f(
    const std::string &name,
    float v0,
    float v1,
    float v2,
    float v3)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniform4f(location, v0, v1, v2, v3);
}

void Program::setMat4f(const std::string &name, const glm::mat4 &matrix)
{
  const GLint location =
      getUniformLocation(name, programHandle_, uniformLocationCache_);
  if (location == invalidLocation)
  {
    return;
  }
  glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

bool Program::hasUniform(const std::string &name) const
{
  if (uniformLocationCache_.contains(name))
  {
    return uniformLocationCache_.at(name) != invalidLocation;
  }

  GLint location = glGetUniformLocation(programHandle_, name.c_str());
  return location != invalidLocation;
}

}  // namespace Simple3D

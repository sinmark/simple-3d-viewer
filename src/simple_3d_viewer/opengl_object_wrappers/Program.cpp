#include <iostream>
#include <optional>
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/utils/fileOperations.hpp>
#include <vector>

namespace Simple3D
{
uint64_t Program::idGenerator_ = 0;

static constexpr auto errorPrefix = "Program error: ";

static constexpr GLint invalidLocation = -1;

void Program::loadShaders(
    const std::string &vertexShaderFileName,
    const std::string &fragmentShaderFileName)
{
  const std::string shaderDirPath = "./res/shaders/";
  vertexShaderCode_ = loadFileIntoString(shaderDirPath + vertexShaderFileName);
  fragmentShaderCode_ =
      loadFileIntoString(shaderDirPath + fragmentShaderFileName);
}

static std::optional<std::string> getCompilationErrorMessage(GLuint shader)
{
  GLint compiled{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled)
  {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes the NULL character
    std::string errorMessage(maxLength, '\0');
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorMessage.front());
    std::cout << errorPrefix << errorMessage << '\n';
    return std::make_optional(std::move(errorMessage));
  }
  return {};
}

void Program::linkProgram()
{
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char *vertexShaderCodePtr = vertexShaderCode_.c_str();
  glShaderSource(vertexShader, 1, &vertexShaderCodePtr, NULL);
  glCompileShader(vertexShader);
  if (const std::optional<std::string> errorMessage =
          getCompilationErrorMessage(vertexShader);
      errorMessage.has_value())
  {
    glDeleteShader(vertexShader);
    throw std::invalid_argument(errorMessage.value());
  }
  glAttachShader(program_, vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fragmentShaderCodePtr = fragmentShaderCode_.c_str();
  glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, NULL);
  glCompileShader(fragmentShader);
  if (const std::optional<std::string> errorMessage =
          getCompilationErrorMessage(fragmentShader);
      errorMessage.has_value())
  {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    throw std::invalid_argument(errorMessage.value());
  }
  glAttachShader(program_, fragmentShader);

  glLinkProgram(program_);
  GLint linked = 0;
  glGetProgramiv(program_, GL_LINK_STATUS, (int *)&linked);
  if (!linked)
  {
    GLint maxLength = 0;
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes the NULL character
    std::string errorMessage(maxLength, '\0');
    glGetShaderInfoLog(
        fragmentShader, maxLength, &maxLength, &errorMessage.front());
    std::cout << errorPrefix << errorMessage << '\n';

    glDeleteProgram(program_);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    throw std::invalid_argument(errorMessage);
  }

  glDetachShader(program_, vertexShader);
  glDetachShader(program_, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

static void printInvalidUniform(const std::string &name)
{
  const std::string log = std::string(errorPrefix) + "Uniform '" + name +
                          "' doesn't exist! Might have been optimized out!";
  std::cout << log << '\n';
}

void Program::setInt(const std::string &name, int value)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
    return;
  glUniform1i(location, value);
}

void Program::setFloat(const std::string &name, float value)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
    return;
  glUniform1f(location, value);
}

void Program::setVec2f(const std::string &name, const glm::vec2 &vector)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
    return;
  glUniform2f(location, vector[0], vector[1]);
}

void Program::setVec2f(const std::string &name, float v0, float v1)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
    return;
  glUniform2f(location, v0, v1);
}

void Program::setVec3f(const std::string &name, const glm::vec3 &vector)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
    return;
  glUniform3f(location, vector[0], vector[1], vector[2]);
}

void Program::setVec3f(const std::string &name, float v0, float v1, float v2)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
    return;
  glUniform3f(location, v0, v1, v2);
}

void Program::setVec4f(const std::string &name, const glm::vec4 &vector)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
    return;
  glUniform4f(location, vector[0], vector[1], vector[2], vector[3]);
}

void Program::setVec4f(
    const std::string &name,
    float v0,
    float v1,
    float v2,
    float v3)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
    return;
  glUniform4f(location, v0, v1, v2, v3);
}

void Program::setMat4f(const std::string &name, const glm::mat4 &matrix)
{
  assert(beingUsed_);
  GLint location = getUniformLocation(name);
  if (location == invalidLocation)
  {
    return;
  }
  glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

bool Program::hasUniform(const std::string &name) const
{
  GLint location = glGetUniformLocation(program_, name.c_str());
  return location != invalidLocation;
}

GLint Program::getUniformLocation(const std::string &name)
{
  const auto uniformLocationIt = uniformLocationCache_.find(name);
  if (uniformLocationIt != uniformLocationCache_.end())
    return uniformLocationIt->second;
  const auto invalidUniformIt = invalidUniformsCache_.find(name);
  if (invalidUniformIt != invalidUniformsCache_.end())
    return invalidLocation;

  GLint location = glGetUniformLocation(program_, name.c_str());
  if (location != invalidLocation)
    uniformLocationCache_.insert({ name, location });
  else
  {
    printInvalidUniform(name);
    invalidUniformsCache_.insert(name);
  }
  return location;
}
}  // namespace Simple3D

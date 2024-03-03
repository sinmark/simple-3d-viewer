#pragma once

#include <filesystem>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

namespace Simple3D
{

inline const std::filesystem::path& kShaderDirPath()
{
  static auto result = std::filesystem::current_path() / "res/shaders/";
  return result;
}

inline const std::filesystem::path& kTexturesDirPath()
{
  static auto result = std::filesystem::current_path() / "res/textures/";
  return result;
}

inline const std::vector<std::filesystem::path>& kSkyboxImagesPaths()
{
  static std::vector<std::filesystem::path> result = {
    kTexturesDirPath() / "skybox/right.jpg",
    kTexturesDirPath() / "skybox/left.jpg",
    kTexturesDirPath() / "skybox/top.jpg",
    kTexturesDirPath() / "skybox/bottom.jpg",
    kTexturesDirPath() / "skybox/front.jpg",
    kTexturesDirPath() / "skybox/back.jpg"
  };
  return result;
}

inline const char* const kLightShaderCommonName = "light";
inline const char* const kSkyboxShaderCommonName = "skybox";
inline const char* const kModelShaderCommonName = "model";

inline constexpr auto kVec3ComponentsCount = glm::vec3::length();
inline constexpr auto kVec2ComponentsCount = glm::vec2::length();

}  // namespace Simple3D
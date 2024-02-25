#pragma once

#include <glad/glad.h>

#include <optional>
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <vector>

namespace Simple3D
{
class Material
{
 public:
  struct TextureData
  {
    Texture *texture;
    std::optional<int> uvChannel;
  };

  Material(
      std::vector<TextureData> &&diffuseTextures,
      std::vector<TextureData> &&specularTextures,
      std::vector<TextureData> &&emissiveTextures,
      std::vector<TextureData> &&normalsTextures,
      std::vector<TextureData> &&metalnessTextures,
      std::vector<TextureData> &&diffuseRoughnessTextures)
      : diffuseTextures_(std::move(diffuseTextures)),
        specularTextures_(std::move(specularTextures)),
        emissiveTextures_(std::move(emissiveTextures)),
        normalsTextures_(std::move(normalsTextures)),
        metalnessTextures_(std::move(metalnessTextures)),
        diffuseRoughnessTextures_(std::move(diffuseRoughnessTextures)),
        id_(idGenerator_++)
  {
  }

  glm::vec3 ambientColor{ 0.f, 0.f, 0.f };
  glm::vec3 diffuseColor{ 0.f, 0.f, 0.f };
  glm::vec3 specularColor{ 0.f, 0.f, 0.f };
  glm::vec3 emissiveColor{ 0.f, 0.f, 0.f };
  float opacity{ 1.f };
  float shininess{ 0.f };
  float shininessStrength{ 1.f };

  void use(Program &program);
  uint64_t getID() const
  {
    return id_;
  }

 private:
  std::vector<TextureData> diffuseTextures_;
  std::vector<TextureData> specularTextures_;
  std::vector<TextureData> emissiveTextures_;
  std::vector<TextureData> normalsTextures_;
  std::vector<TextureData> metalnessTextures_;
  std::vector<TextureData> diffuseRoughnessTextures_;
  const uint64_t id_;

  static uint64_t idGenerator_;

  void prepareTexturesForUse(Program &program);
  void setUniforms(Program &program);
};
}  // namespace Simple3D

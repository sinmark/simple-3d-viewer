#pragma once

#include <glad/glad.h>

#include "simple_3d_viewer/utils/simpleIdGenerator.hpp"
#include <optional>
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <vector>

namespace Simple3D
{

struct Material
{
 public:
  struct TextureData
  {
    Texture* texture;
    std::optional<int> uvChannel;
  };

  Material(
      std::vector<TextureData> pDiffuseTextures,
      std::vector<TextureData> pSpecularTextures,
      std::vector<TextureData> pEmissiveTextures,
      std::vector<TextureData> pNormalsTextures,
      std::vector<TextureData> pMetalnessTextures,
      std::vector<TextureData> pDiffuseRoughnessTextures)
      : diffuseTextures(std::move(pDiffuseTextures)),
        specularTextures(std::move(pSpecularTextures)),
        emissiveTextures(std::move(pEmissiveTextures)),
        normalsTextures(std::move(pNormalsTextures)),
        metalnessTextures(std::move(pMetalnessTextures)),
        diffuseRoughnessTextures(std::move(pDiffuseRoughnessTextures))
  {
  }

  glm::vec3 ambientColor{ 0.f, 0.f, 0.f };
  glm::vec3 diffuseColor{ 0.f, 0.f, 0.f };
  glm::vec3 specularColor{ 0.f, 0.f, 0.f };
  glm::vec3 emissiveColor{ 0.f, 0.f, 0.f };
  float opacity{ 1.f };
  float shininess{ 0.f };
  float shininessStrength{ 1.f };
  std::vector<TextureData> diffuseTextures;
  std::vector<TextureData> specularTextures;
  std::vector<TextureData> emissiveTextures;
  std::vector<TextureData> normalsTextures;
  std::vector<TextureData> metalnessTextures;
  std::vector<TextureData> diffuseRoughnessTextures;
  uint64_t id = generateSimpleId();

  void use(Program& program);

  [[nodiscard]] uint64_t getId() const
  {
    return id;
  }
};

}  // namespace Simple3D

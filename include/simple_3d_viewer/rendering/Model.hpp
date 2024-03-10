#pragma once

#include <glad/glad.h>

#include "simple_3d_viewer/utils/simpleIdGenerator.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <bitset>
#include <cstddef>
#include <filesystem>
#include <glm/glm.hpp>
#include <simple_3d_viewer/linear_algebra/Transform.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/rendering/Material.hpp>
#include <simple_3d_viewer/rendering/Mesh.hpp>
#include <stb_image.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace Simple3D
{

class Model
{
 public:
  class Configuration
  {
   public:
    enum class Flag
    {
      FlipUVs,
      FlagsCount,
    };

    void set(Flag flag, bool value)
    {
      if (flag == Flag::FlagsCount)
      {
        throw std::invalid_argument("FlagsCount should never be used");
      }

      flags_.set(static_cast<size_t>(flag), value);
    }

    [[nodiscard]] bool get(Flag flag) const
    {
      if (flag == Flag::FlagsCount)
      {
        throw std::invalid_argument("FlagsCount should never be used");
      }

      return flags_[static_cast<uint32_t>(flag)];
    }

    [[nodiscard]] unsigned int getEquivalentAssimpFlags() const
    {
      unsigned int flags = 0;
      for (const auto& [flag, assimpFlag] : flagToAssimpFlag_)
      {
        flags = flags | (flags_[static_cast<size_t>(flag)] ? assimpFlag : 0);
      }
      return flags;
    }

   private:
    std::bitset<static_cast<size_t>(Flag::FlagsCount)> flags_{};
    static const std::unordered_map<Flag, aiPostProcessSteps> flagToAssimpFlag_;
  };

  Model(
      const std::filesystem::path& modelFilePath,
      const Configuration& configuration)
  {
    loadModel(modelFilePath, configuration);
  }

  glm::mat4x4 transform_{ calculateModelTransform(
      { { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } }) };
  std::vector<Mesh> meshes_;
  std::vector<Material> materials_;
  std::vector<Texture> textures_;

  void complete()
  {
    for (auto& texture : textures_)
    {
      texture.complete();
    }
    for (auto& mesh : meshes_)
    {
      mesh.complete();
    }
  }

  void setTransform(const Transform& transform)
  {
    transform_ = calculateModelTransform(transform);
  }

  [[nodiscard]] uint64_t getId() const
  {
    return id_;
  }

 private:
  uint64_t id_ = generateSimpleId();

  void loadModel(
      const std::filesystem::path& modelFilePath,
      const Configuration& configuration);
  void processMaterials(
      const aiScene& scene,
      const std::filesystem::path& modelDirectory);
  void processNode(const aiNode& node, const aiScene& scene);
  void loadMaterialTextures(
      const aiMaterial& assimpMaterial,
      const std::filesystem::path& modelDirectory);
  void loadMaterialTexturesOfType(
      const aiMaterial& assimpMaterial,
      aiTextureType type,
      const std::filesystem::path& modelDirectory);
  Material processMaterial(
      const aiMaterial& assimpMaterial,
      const std::filesystem::path& modelDirectory);
  std::vector<Material::TextureData> getBelongingTextures(
      const aiMaterial& assimpMaterial,
      aiTextureType type,
      const std::filesystem::path& modelDirectory);
  Mesh processMesh(const aiMesh& assimpMesh);
};
}  // namespace Simple3D

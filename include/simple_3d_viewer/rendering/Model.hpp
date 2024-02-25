#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <bitset>
#include <glm/glm.hpp>
#include <simple_3d_viewer/linear_algebra/Transform.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/rendering/Material.hpp>
#include <simple_3d_viewer/rendering/Mesh.hpp>
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
      IssueRenderingAPICalls,
      FlipUVs,
      FlagsCount,
    };

    void set(Flag flag, bool value)
    {
      flags_.set(static_cast<uint32_t>(flag), value);
    }
    bool get(Flag flag) const
    {
      return flags_[static_cast<uint32_t>(flag)];
    }
    unsigned int getEquivalentAssimpFlags() const
    {
      unsigned int flags = 0;
      for (const auto &[flag, assimpFlag] : flagToAssimpFlag_)
        flags = flags | (flags_[static_cast<uint32_t>(flag)] ? assimpFlag : 0);
      return flags;
    }

   private:
    std::bitset<static_cast<size_t>(Flag::FlagsCount)> flags_{};
    static const std::unordered_map<Flag, aiPostProcessSteps> flagToAssimpFlag_;
  };

  Model(const std::string &pathToFile, const Configuration &configuration)
      : id_(idGenerator_++),
        configuration_(configuration)
  {
    loadModel(pathToFile);
  }

  glm::mat4x4 transform_{ calculateModelTransform(
      { { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } }) };
  std::vector<Mesh> meshes_;
  std::vector<Material> materials_;
  std::vector<Texture> textures_;

  void complete()
  {
    for (auto &texture : textures_)
      texture.complete();
    for (auto &mesh : meshes_)
      mesh.complete();
  }
  void setTransform(const Transform &transform)
  {
    transform_ = calculateModelTransform(transform);
  }
  uint64_t getID() const
  {
    return id_;
  }

 private:
  Configuration configuration_;
  std::string modelDirectory_;
  uint64_t id_;

  static uint64_t idGenerator_;

  void loadModel(const std::string &path);
  void processMaterials(const aiScene *scene);
  void processNode(aiNode *node, const aiScene *scene);
  void loadMaterialTextures(aiMaterial *assimpMaterial);
  void loadMaterialTexturesOfType(
      aiMaterial *assimpMaterial,
      aiTextureType type);
  Material processMaterial(aiMaterial *assimpMaterial);
  std::vector<Material::TextureData> getBelongingTextures(
      aiMaterial *assimpMaterial,
      aiTextureType type);
  Mesh processMesh(aiMesh *assimpMesh, const aiScene *scene);
};
}  // namespace Simple3D

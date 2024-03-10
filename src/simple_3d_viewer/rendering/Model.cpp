#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <simple_3d_viewer/linear_algebra/Vertex.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/rendering/Material.hpp>
#include <simple_3d_viewer/rendering/Model.hpp>
#include <stdexcept>

namespace Simple3D
{

namespace
{

const char* const kErrorPrefix = "Error (Model):";

}

const std::unordered_map<Model::Configuration::Flag, aiPostProcessSteps>
    Model::Configuration::flagToAssimpFlag_ = {
      { Configuration::Flag::FlipUVs, aiPostProcessSteps::aiProcess_FlipUVs }
    };

void Model::loadModel(
    const std::filesystem::path& modelFilePath,
    const Model::Configuration& configuration)
{
  Assimp::Importer importer;
  const aiScene* scenePtr = importer.ReadFile(
      modelFilePath,
      aiProcess_Triangulate | aiProcess_GenSmoothNormals |
          aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices |
          configuration.getEquivalentAssimpFlags());
  if ((scenePtr == nullptr) ||
      ((scenePtr->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0u) ||
      (scenePtr->mRootNode == nullptr))
  {
    throw std::invalid_argument(
        fmt::format("{} {}", kErrorPrefix, importer.GetErrorString()));
  }
  const auto modelDirectory = modelFilePath.parent_path();
  const auto& scene = *scenePtr;

  if (scene.HasMaterials())
  {
    processMaterials(scene, modelDirectory);
  }
  processNode(*scene.mRootNode, scene);
}

void Model::processMaterials(
    const aiScene& scene,
    const std::filesystem::path& modelDirectory)
{
  const auto materialsCount = scene.mNumMaterials;
  materials_.reserve(materialsCount);
  for (auto i = decltype(materialsCount){}; i < materialsCount; ++i)
  {
    const aiMaterial& assimpMaterial = *scene.mMaterials[i];
    loadMaterialTextures(assimpMaterial, modelDirectory);
  }
  for (auto i = decltype(materialsCount){}; i < materialsCount; ++i)
  {
    const aiMaterial& assimpMaterial = *scene.mMaterials[i];
    materials_.emplace_back(processMaterial(assimpMaterial, modelDirectory));
  }
}

void Model::processNode(const aiNode& node, const aiScene& scene)
{
  const auto meshesCount = node.mNumMeshes;
  for (auto i = decltype(meshesCount){}; i < meshesCount; ++i)
  {
    const aiMesh& assimpMesh = *scene.mMeshes[node.mMeshes[i]];
    meshes_.emplace_back(processMesh(assimpMesh));
  }
  const auto childrenCount = node.mNumChildren;
  for (auto i = decltype(childrenCount){}; i < childrenCount; ++i)
  {
    processNode(*node.mChildren[i], scene);
  }
}

void Model::loadMaterialTextures(
    const aiMaterial& assimpMaterial,
    const std::filesystem::path& modelDirectory)
{
  loadMaterialTexturesOfType(
      assimpMaterial, aiTextureType_DIFFUSE, modelDirectory);
  loadMaterialTexturesOfType(
      assimpMaterial, aiTextureType_SPECULAR, modelDirectory);
  loadMaterialTexturesOfType(
      assimpMaterial, aiTextureType_EMISSIVE, modelDirectory);
  loadMaterialTexturesOfType(
      assimpMaterial, aiTextureType_NORMALS, modelDirectory);
  loadMaterialTexturesOfType(
      assimpMaterial, aiTextureType_METALNESS, modelDirectory);
  loadMaterialTexturesOfType(
      assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS, modelDirectory);
}

Material Model::processMaterial(
    const aiMaterial& assimpMaterial,
    const std::filesystem::path& modelDirectory)
{
  std::vector<Material::TextureData> diffuseTextures = getBelongingTextures(
      assimpMaterial, aiTextureType_DIFFUSE, modelDirectory);
  std::vector<Material::TextureData> specularTextures = getBelongingTextures(
      assimpMaterial, aiTextureType_SPECULAR, modelDirectory);
  std::vector<Material::TextureData> emissiveTextures = getBelongingTextures(
      assimpMaterial, aiTextureType_EMISSIVE, modelDirectory);
  std::vector<Material::TextureData> normalsTextures = getBelongingTextures(
      assimpMaterial, aiTextureType_NORMALS, modelDirectory);
  std::vector<Material::TextureData> metalnessTextures = getBelongingTextures(
      assimpMaterial, aiTextureType_METALNESS, modelDirectory);
  std::vector<Material::TextureData> diffuseRoughnessTextures =
      getBelongingTextures(
          assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS, modelDirectory);
  Material material(
      std::move(diffuseTextures),
      std::move(specularTextures),
      std::move(emissiveTextures),
      std::move(normalsTextures),
      std::move(metalnessTextures),
      std::move(diffuseRoughnessTextures));

  aiColor3D colorAmbient(0.f, 0.f, 0.f);
  assimpMaterial.Get(AI_MATKEY_COLOR_AMBIENT, colorAmbient);
  material.ambientColor =
      glm::vec3(colorAmbient.r, colorAmbient.g, colorAmbient.b);

  aiColor3D colorDiffuse(0.f, 0.f, 0.f);
  assimpMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, colorDiffuse);
  material.diffuseColor =
      glm::vec3(colorDiffuse.r, colorDiffuse.g, colorDiffuse.b);

  aiColor3D colorSpecular(0.f, 0.f, 0.f);
  assimpMaterial.Get(AI_MATKEY_COLOR_SPECULAR, colorSpecular);
  material.specularColor =
      glm::vec3(colorSpecular.r, colorSpecular.g, colorSpecular.b);

  aiColor3D colorEmissive(0.f, 0.f, 0.f);
  assimpMaterial.Get(AI_MATKEY_COLOR_EMISSIVE, colorEmissive);
  material.specularColor =
      glm::vec3(colorEmissive.r, colorEmissive.g, colorEmissive.b);

  assimpMaterial.Get(AI_MATKEY_OPACITY, material.opacity);
  assimpMaterial.Get(AI_MATKEY_SHININESS, material.shininess);
  assimpMaterial.Get(AI_MATKEY_SHININESS_STRENGTH, material.shininessStrength);

  return material;
}

void Model::loadMaterialTexturesOfType(
    const aiMaterial& assimpMaterial,
    aiTextureType type,
    const std::filesystem::path& modelDirectory)
{
  const auto texturesCount = assimpMaterial.GetTextureCount(type);
  for (auto i = decltype(texturesCount){}; i < texturesCount; ++i)
  {
    aiString texturePath;
    assimpMaterial.GetTexture(type, i, &texturePath);
    const std::filesystem::path relativePathToTexture(texturePath.C_Str());
    const auto pathToTexture = modelDirectory / relativePathToTexture;

    const auto it = std::ranges::find_if(
        textures_,
        [&pathToTexture](const Texture& texture)
        { return pathToTexture == texture.getPath(); });
    if (it != textures_.end())
    {
      continue;
    }

    textures_.emplace_back(pathToTexture, false);
  }
}

std::vector<Material::TextureData> Model::getBelongingTextures(
    const aiMaterial& assimpMaterial,
    aiTextureType type,
    const std::filesystem::path& modelDirectory)
{
  std::vector<Material::TextureData> textures;
  const auto texturesCount = assimpMaterial.GetTextureCount(type);
  for (auto i = decltype(texturesCount){}; i < texturesCount; ++i)
  {
    aiString texturePath;
    assimpMaterial.GetTexture(type, i, &texturePath);
    const std::filesystem::path relativePathToTexture(texturePath.C_Str());
    const auto pathToTexture = modelDirectory / relativePathToTexture;

    const auto it = std::ranges::find_if(
        textures_,
        [&pathToTexture](const Texture& texture)
        { return pathToTexture == texture.getPath(); });
    if (it == end(textures_))
    {
      throw std::logic_error("Textures should be populated by this point");
    }

    int uvChannel{};
    auto& texture = *it;
    if (assimpMaterial.Get(AI_MATKEY_UVWSRC(type, i), uvChannel) == AI_SUCCESS)
    {
      textures.push_back(
          { &texture,
            std::make_optional(
                uvChannel < static_cast<int>(Vertex::maxUVChannels) ? uvChannel
                                                                    : 0) });
    }
    else
    {
      textures.push_back({ &texture, {} });
    }
  }

  return textures;
}

Mesh Model::processMesh(const aiMesh& assimpMesh)
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  Material* material = nullptr;

  const auto uvChannelsCount = assimpMesh.GetNumUVChannels();
  const auto uvChannelsCountClamped =
      std::clamp(uvChannelsCount, {}, Vertex::maxUVChannels);
  const auto verticesCount = assimpMesh.mNumVertices;
  for (auto i = decltype(verticesCount){}; i < verticesCount; ++i)
  {
    Vertex vertex{};
    glm::vec3 vector{};
    vector.x = assimpMesh.mVertices[i].x;
    vector.y = assimpMesh.mVertices[i].y;
    vector.z = assimpMesh.mVertices[i].z;
    vertex.position = vector;
    if (assimpMesh.HasNormals())
    {
      vector.x = assimpMesh.mNormals[i].x;
      vector.y = assimpMesh.mNormals[i].y;
      vector.z = assimpMesh.mNormals[i].z;
      vertex.normal = vector;
    }
    if (assimpMesh.HasVertexColors(0))
    {
      vector.r = assimpMesh.mColors[0][i].r;
      vector.g = assimpMesh.mColors[0][i].g;
      vector.b = assimpMesh.mColors[0][i].b;
      vertex.color = vector;
    }
    glm::vec2 vector2D{};
    for (auto j = decltype(uvChannelsCountClamped){};
         j < uvChannelsCountClamped;
         ++j)
    {
      vector2D.x = assimpMesh.mTextureCoords[j][i].x;
      vector2D.y = assimpMesh.mTextureCoords[j][i].y;
      vertex.texCoords[j] = vector2D;
    }

    vertices.push_back(vertex);
  }

  const auto facesCount = assimpMesh.mNumFaces;
  for (auto i = decltype(facesCount){}; i < facesCount; i++)
  {
    const aiFace& assimpFace = assimpMesh.mFaces[i];
    for (size_t j = 0; j < assimpFace.mNumIndices; ++j)
    {
      indices.push_back(assimpFace.mIndices[j]);
    }
  }

  material = &materials_[assimpMesh.mMaterialIndex];

  return { std::move(vertices), std::move(indices), material, false };
}

}  // namespace Simple3D

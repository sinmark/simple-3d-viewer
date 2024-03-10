#include <cassert>
#include <iostream>
#include <simple_3d_viewer/linear_algebra/Vertex.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/rendering/Material.hpp>
#include <simple_3d_viewer/rendering/Model.hpp>

namespace Simple3D
{
uint64_t Model::idGenerator_ = 0;

const std::unordered_map<Model::Configuration::Flag, aiPostProcessSteps>
    Model::Configuration::flagToAssimpFlag_ = {
      { Configuration::Flag::FlipUVs, aiPostProcessSteps::aiProcess_FlipUVs }
    };

void Model::loadModel(const std::filesystem::path& modelFilePath)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      modelFilePath,
      aiProcess_Triangulate | aiProcess_GenSmoothNormals |
          aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices |
          configuration_.getEquivalentAssimpFlags());
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    const auto errorMessage =
        "Assimp error: " + std::string(importer.GetErrorString());
    throw std::invalid_argument(errorMessage);
  }
  modelDirectory_ = modelFilePath.parent_path();

  if (scene->HasMaterials())
  {
    processMaterials(scene);
  }
  processNode(scene->mRootNode, scene);
}

void Model::processMaterials(const aiScene* scene)
{
  const uint32_t materialsCount = scene->mNumMaterials;
  materials_.reserve(materialsCount);
  for (uint32_t i = 0; i < materialsCount; ++i)
  {
    aiMaterial* assimpMaterial = scene->mMaterials[i];
    loadMaterialTextures(assimpMaterial);
  }
  for (uint32_t i = 0; i < materialsCount; ++i)
  {
    aiMaterial* assimpMaterial = scene->mMaterials[i];
    materials_.emplace_back(processMaterial(assimpMaterial));
  }
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
  const uint32_t meshesCount = node->mNumMeshes;
  for (uint32_t i = 0; i < node->mNumMeshes; ++i)
  {
    aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.emplace_back(processMesh(assimpMesh, scene));
  }
  const uint32_t childrenCount = node->mNumChildren;
  for (uint32_t i = 0; i < childrenCount; ++i)
  {
    processNode(node->mChildren[i], scene);
  }
}

void Model::loadMaterialTextures(aiMaterial* assimpMaterial)
{
  loadMaterialTexturesOfType(assimpMaterial, aiTextureType_DIFFUSE);
  loadMaterialTexturesOfType(assimpMaterial, aiTextureType_SPECULAR);
  loadMaterialTexturesOfType(assimpMaterial, aiTextureType_EMISSIVE);
  loadMaterialTexturesOfType(assimpMaterial, aiTextureType_NORMALS);
  loadMaterialTexturesOfType(assimpMaterial, aiTextureType_METALNESS);
  loadMaterialTexturesOfType(assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS);
}

Material Model::processMaterial(aiMaterial* assimpMaterial)
{
  std::vector<Material::TextureData> diffuseTextures =
      getBelongingTextures(assimpMaterial, aiTextureType_DIFFUSE);
  std::vector<Material::TextureData> specularTextures =
      getBelongingTextures(assimpMaterial, aiTextureType_SPECULAR);
  std::vector<Material::TextureData> emissiveTextures =
      getBelongingTextures(assimpMaterial, aiTextureType_EMISSIVE);
  std::vector<Material::TextureData> normalsTextures =
      getBelongingTextures(assimpMaterial, aiTextureType_NORMALS);
  std::vector<Material::TextureData> metalnessTextures =
      getBelongingTextures(assimpMaterial, aiTextureType_METALNESS);
  std::vector<Material::TextureData> diffuseRoughnessTextures =
      getBelongingTextures(assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS);
  Material material(
      std::move(diffuseTextures),
      std::move(specularTextures),
      std::move(emissiveTextures),
      std::move(normalsTextures),
      std::move(metalnessTextures),
      std::move(diffuseRoughnessTextures));

  aiColor3D colorAmbient(0.f, 0.f, 0.f);
  assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, colorAmbient);
  material.ambientColor =
      glm::vec3(colorAmbient.r, colorAmbient.g, colorAmbient.b);

  aiColor3D colorDiffuse(0.f, 0.f, 0.f);
  assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, colorDiffuse);
  material.diffuseColor =
      glm::vec3(colorDiffuse.r, colorDiffuse.g, colorDiffuse.b);

  aiColor3D colorSpecular(0.f, 0.f, 0.f);
  assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, colorSpecular);
  material.specularColor =
      glm::vec3(colorSpecular.r, colorSpecular.g, colorSpecular.b);

  aiColor3D colorEmissive(0.f, 0.f, 0.f);
  assimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, colorEmissive);
  material.specularColor =
      glm::vec3(colorEmissive.r, colorEmissive.g, colorEmissive.b);

  assimpMaterial->Get(AI_MATKEY_OPACITY, material.opacity);
  assimpMaterial->Get(AI_MATKEY_SHININESS, material.shininess);
  assimpMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, material.shininessStrength);

  return material;
}

void Model::loadMaterialTexturesOfType(
    aiMaterial* assimpMaterial,
    aiTextureType type)
{
  const uint32_t texturesCount = assimpMaterial->GetTextureCount(type);
  for (uint32_t i = 0; i < texturesCount; ++i)
  {
    aiString texturePath;
    assimpMaterial->GetTexture(type, i, &texturePath);
    const std::string relativePathToTexture(texturePath.C_Str());
    const std::string pathToTexture =
        modelDirectory_ + "/" + relativePathToTexture;

    const auto it = std::find_if(
        textures_.begin(),
        textures_.end(),
        [&pathToTexture](const Texture& texture)
        { return pathToTexture == texture.getPath(); });
    if (it != textures_.end())
      continue;
    textures_.emplace_back(Texture(
        pathToTexture,
        configuration_.get(Configuration::Flag::IssueRenderingAPICalls)));
  }
}

std::vector<Material::TextureData> Model::getBelongingTextures(
    aiMaterial* assimpMaterial,
    aiTextureType type)
{
  std::vector<Material::TextureData> textures;
  const uint32_t texturesCount = assimpMaterial->GetTextureCount(type);
  for (uint32_t i = 0; i < texturesCount; ++i)
  {
    aiString texturePath;
    assimpMaterial->GetTexture(type, i, &texturePath);
    const std::string relativePathToTexture(texturePath.C_Str());
    const std::string pathToTexture =
        modelDirectory_ + "/" + relativePathToTexture;

    const auto it = std::find_if(
        textures_.begin(),
        textures_.end(),
        [&pathToTexture](const Texture& texture)
        { return pathToTexture == texture.getPath(); });
    assert(it != textures_.end());
    int uvChannel;
    auto& texture = *it;
    if (assimpMaterial->Get(AI_MATKEY_UVWSRC(type, i), uvChannel) == AI_SUCCESS)
      textures.push_back(
          { &texture,
            std::make_optional(
                uvChannel < Vertex::maxUVChannels ? uvChannel : 0) });
    else
      textures.push_back({ &texture, {} });
  }
  return textures;
}

Mesh Model::processMesh(aiMesh* assimpMeshPtr, const aiScene* scene)
{
  assert(assimpMeshPtr != nullptr);

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  Material* material = nullptr;
  const auto& assimpMesh = *assimpMeshPtr;

  const size_t uvChannelsCount = assimpMesh.GetNumUVChannels();
  const size_t uvChannelsCountClamped = std::clamp(
      uvChannelsCount, static_cast<size_t>(0), Vertex::maxUVChannels);
  const size_t verticesCount = assimpMesh.mNumVertices;
  for (size_t i = 0; i < verticesCount; ++i)
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
    for (size_t j = 0; j < uvChannelsCountClamped; ++j)
    {
      vector2D.x = assimpMesh.mTextureCoords[j][i].x;
      vector2D.y = assimpMesh.mTextureCoords[j][i].y;
      vertex.texCoords[j] = vector2D;
    }

    vertices.push_back(vertex);
  }

  const size_t facesCount = assimpMesh.mNumFaces;
  for (size_t i = 0; i < facesCount; i++)
  {
    const aiFace& assimpFace = assimpMesh.mFaces[i];
    for (size_t j = 0; j < assimpFace.mNumIndices; ++j)
    {
      indices.push_back(assimpFace.mIndices[j]);
    }
  }

  if (assimpMesh.mMaterialIndex >= 0)
  {
    aiMaterial* aimaterial = scene->mMaterials[assimpMesh.mMaterialIndex];
    material = &materials_[assimpMesh.mMaterialIndex];
  }

  return { std::move(vertices),
           std::move(indices),
           material,
           configuration_.get(Configuration::Flag::IssueRenderingAPICalls) };
}
}  // namespace Simple3D

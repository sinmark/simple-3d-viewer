#include <simple_3d_viewer/rendering/Material.hpp>

namespace Simple3D
{
uint64_t Material::idGenerator_ = 0;

void Material::use(Program& program)
{
  prepareTexturesForUse(program);
  setUniforms(program);
}

static inline void setSamplerUniform(
    Program& program,
    const std::string& uniformNameBase,
    uint32_t concatNumber,
    uint32_t slot)
{
  const std::string uniformName =
      uniformNameBase + std::to_string(concatNumber);
  program.doOperations([&uniformName, slot](Program& program)
                       { program.setInt(uniformName, slot); });
}

static inline void setUVChannelUniform(
    Program& program,
    const std::string& uniformNameBase,
    uint32_t concatNumber,
    const std::optional<int>& uvChannel)
{
  const int uvChannelValue = uvChannel.value_or(concatNumber);
  const std::string uniformName =
      uniformNameBase + std::to_string(concatNumber);
  program.doOperations([&uniformName, uvChannelValue](Program& program)
                       { program.setInt(uniformName, uvChannelValue); });
}

static void assignSlotsToTextures(
    std::vector<Material::TextureData>& textures,
    const std::string& texturesType,
    Program& program,
    uint32_t& slotToAssign)
{
  const size_t texturesCount = textures.size();
  for (uint32_t i = 0; i < texturesCount; ++i)
  {
    uint32_t slot = slotToAssign++;
    auto& textureData = textures[i];
    textureData.texture->use(slot);
    setSamplerUniform(program, texturesType + "Texture", i, slot);
    setUVChannelUniform(
        program, texturesType + "UVChannel", i, textureData.uvChannel);
  }
}

void Material::prepareTexturesForUse(Program& program)
{
  uint32_t slotToAssign = 0;
  assignSlotsToTextures(diffuseTextures_, "diffuse", program, slotToAssign);
  assignSlotsToTextures(specularTextures_, "specular", program, slotToAssign);
  assignSlotsToTextures(emissiveTextures_, "emissive", program, slotToAssign);
  assignSlotsToTextures(normalsTextures_, "normals", program, slotToAssign);
  assignSlotsToTextures(metalnessTextures_, "metalness", program, slotToAssign);
  assignSlotsToTextures(
      diffuseRoughnessTextures_, "diffuseRoughness", program, slotToAssign);
}

void Material::setUniforms(Program& program)
{
  program.doOperations(
      [this](Program& program)
      {
        program.setVec3f("ambientColor", ambientColor);
        program.setVec3f("diffuseColor", diffuseColor);
        program.setVec3f("specularColor", specularColor);
        program.setVec3f("emissiveColor", emissiveColor);

        program.setFloat("shininess", shininess);
        program.setFloat("shininessStrength", shininessStrength);

        program.setInt(
            "diffuseTexturesCount", static_cast<int>(diffuseTextures_.size()));
        program.setInt(
            "specularTexturesCount",
            static_cast<int>(specularTextures_.size()));
        program.setInt(
            "emissiveTexturesCount",
            static_cast<int>(emissiveTextures_.size()));
        program.setInt(
            "normalsTexturesCount", static_cast<int>(normalsTextures_.size()));
        program.setInt(
            "metalnessTexturesCount",
            static_cast<int>(metalnessTextures_.size()));
        program.setInt(
            "diffuseRoughnessTexturesCount",
            static_cast<int>(diffuseRoughnessTextures_.size()));
      });
}
}  // namespace Simple3D

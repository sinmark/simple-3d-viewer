#include <cstdint>
#include <fmt/format.h>
#include <simple_3d_viewer/rendering/Material.hpp>
#include <sys/types.h>

namespace Simple3D
{

namespace
{

void setUniforms(Program& program, const Material& material)
{
  program.doOperations(
      [&material](Program& it)
      {
        it.setVec3f("ambientColor", material.ambientColor);
        it.setVec3f("diffuseColor", material.diffuseColor);
        it.setVec3f("specularColor", material.specularColor);
        it.setVec3f("emissiveColor", material.emissiveColor);
        it.setFloat("shininess", material.shininess);
        it.setFloat("shininessStrength", material.shininessStrength);
        it.setInt(
            "diffuseTexturesCount",
            static_cast<int>(material.diffuseTextures.size()));
        it.setInt(
            "specularTexturesCount",
            static_cast<int>(material.specularTextures.size()));
        it.setInt(
            "emissiveTexturesCount",
            static_cast<int>(material.emissiveTextures.size()));
        it.setInt(
            "normalsTexturesCount",
            static_cast<int>(material.normalsTextures.size()));
        it.setInt(
            "metalnessTexturesCount",
            static_cast<int>(material.metalnessTextures.size()));
        it.setInt(
            "diffuseRoughnessTexturesCount",
            static_cast<int>(material.diffuseRoughnessTextures.size()));
      });
}

void setSamplerUniform(
    Program& program,
    uint32_t concatNumber,
    const std::string& uniformNameBase,
    uint32_t slot)
{
  const std::string uniformName =
      fmt::format("{}{}", uniformNameBase, concatNumber);
  program.doOperations([&uniformName, slot](Program& it)
                       { it.setInt(uniformName, static_cast<int>(slot)); });
}

void setUVChannelUniform(
    Program& program,
    const std::string& uniformNameBase,
    uint32_t concatNumber,
    const std::optional<int>& uvChannel)
{
  const int uvChannelValue = uvChannel.value_or(concatNumber);
  const std::string uniformName =
      fmt::format("{}{}", uniformNameBase, concatNumber);
  program.doOperations([&uniformName, uvChannelValue](Program& it)
                       { it.setInt(uniformName, uvChannelValue); });
}

void assignSlotsToTextures(
    std::vector<Material::TextureData>& textures,
    const std::string& texturesType,
    Program& program,
    uint32_t& slotToAssign)
{
  const size_t texturesCount = textures.size();
  for (uint32_t i = 0; i < texturesCount; ++i)
  {
    auto slot = slotToAssign;
    ++slotToAssign;
    auto& textureData = textures[i];
    textureData.texture->use(slot);
    setSamplerUniform(program, i, texturesType + "Texture", slot);
    setUVChannelUniform(
        program, texturesType + "UVChannel", i, textureData.uvChannel);
  }
}

void prepareTexturesForUse(Program& program, Material& material)
{
  uint32_t slotToAssign = 0;
  assignSlotsToTextures(
      material.diffuseTextures, "diffuse", program, slotToAssign);
  assignSlotsToTextures(
      material.specularTextures, "specular", program, slotToAssign);
  assignSlotsToTextures(
      material.emissiveTextures, "emissive", program, slotToAssign);
  assignSlotsToTextures(
      material.normalsTextures, "normals", program, slotToAssign);
  assignSlotsToTextures(
      material.metalnessTextures, "metalness", program, slotToAssign);
  assignSlotsToTextures(
      material.diffuseRoughnessTextures,
      "diffuseRoughness",
      program,
      slotToAssign);
}

}  // namespace

void Material::use(Program& program)
{
  prepareTexturesForUse(program, *this);
  setUniforms(program, *this);
}

}  // namespace Simple3D

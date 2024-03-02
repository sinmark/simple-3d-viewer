#pragma once

#include "simple_3d_viewer/utils/constants.hpp"
#include "simple_3d_viewer/utils/fileOperations.hpp"
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>

namespace Simple3D
{

inline Program createProgram(
    const std::string& vertexShaderName,
    const std::string& fragmentShaderName)
{
  const auto vertexShader =
      loadFileIntoString(kShaderDirPath() / (vertexShaderName + ".vs"));
  const auto fragmentShader =
      loadFileIntoString(kShaderDirPath() / (fragmentShaderName + ".fs"));

  return { vertexShader, fragmentShader };
}

inline Program createProgram(const std::string& commonShaderName)
{
  return createProgram(commonShaderName, commonShaderName);
}

};  // namespace Simple3D
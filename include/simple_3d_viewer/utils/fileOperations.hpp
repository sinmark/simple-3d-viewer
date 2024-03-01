#pragma once

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <string>
#include <tl/expected.hpp>

namespace Simple3D
{

inline std::string loadFileIntoString(const std::filesystem::path& filePath)
{
  std::ifstream in(filePath, std::ios::in | std::ios::binary);
  if (!in.is_open())
  {
    throw std::invalid_argument(fmt::format(
        "File at path: {} doesn't exist",
        std::filesystem::absolute(filePath).string()));
  }

  std::string file;
  in.seekg(0, std::ios::end);
  file.resize(in.tellg());
  in.seekg(0, std::ios::beg);
  in.read(file.data(), static_cast<long>(file.size()));
  in.close();
  return file;
}

}  // namespace Simple3D

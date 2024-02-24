#pragma once

#include <fstream>
#include <string>
#include <filesystem>

namespace Simple3D {
inline std::string loadFileIntoString(const std::string &filePath) {
  std::ifstream in(filePath, std::ios::in | std::ios::binary);
  if (in) {
    std::string file;
    in.seekg(0, std::ios::end);
    file.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&file[0], file.size());
    in.close();
    return file;
  } else {
    const std::filesystem::path path(filePath);
    const std::string errorMessage =
        "Shader at path: " + std::filesystem::absolute(path).string() +
        " doesn't exist!";
    throw std::invalid_argument(errorMessage);
  }
}
} // namespace Simple3D

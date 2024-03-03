#pragma once

#include <filesystem>
#include <simple_3d_viewer/utils/Image.hpp>
#include <string>

namespace Simple3D
{

std::string loadFileIntoString(const std::filesystem::path& filePath);

Image loadImage(const std::filesystem::path& filePath, bool flipVertically);

}  // namespace Simple3D

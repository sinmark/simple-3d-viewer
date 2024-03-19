#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <simple_3d_viewer/utils/fileOperations.hpp>
#include <stb_image.h>
#include <string>

namespace Simple3D
{

std::string loadFileIntoString(const std::filesystem::path& filePath)
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
  file.resize(static_cast<size_t>(in.tellg()));
  in.seekg(0, std::ios::beg);
  in.read(file.data(), static_cast<long>(file.size()));
  in.close();
  return file;
}

Image loadImage(
    const std::filesystem::path& filePath,
    const bool flipVertically)
{
  int width{};
  int height{};
  int colorChannelCount{};
  if (flipVertically)
  {
    stbi_set_flip_vertically_on_load(1);
  }
  else
  {
    stbi_set_flip_vertically_on_load(0);
  }
  uint8_t* image =
      stbi_load(filePath.c_str(), &width, &height, &colorChannelCount, 0);
  if (image == nullptr)
  {
    throw std::invalid_argument(
        fmt::format("Failed to load image at path: {}", filePath.c_str()));
  }
  return { image, { width, height }, colorChannelCount };
}

}  // namespace Simple3D

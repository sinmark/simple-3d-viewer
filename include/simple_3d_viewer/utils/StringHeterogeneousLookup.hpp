#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Simple3D
{

// Copied over from SonarLint rule description
struct StringHeterogeneousLookupHash
{
  using is_transparent = void;  // enables heterogeneous lookup

  std::size_t operator()(std::string_view sv) const
  {
    std::hash<std::string_view> hasher;
    return hasher(sv);
  }
};

template<typename ValueType>
using StringHeterogeneousLookupUnorderedMap = std::unordered_map<
    std::string,
    ValueType,
    StringHeterogeneousLookupHash,
    std::equal_to<>>;

}  // namespace Simple3D
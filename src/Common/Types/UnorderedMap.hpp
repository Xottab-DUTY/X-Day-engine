#pragma once
#include "Allocator.hpp"
#include <unordered_map>

namespace XDay
{
template <typename key, class value, class hasher = std::hash<key>, class traits = std::equal_to<key>,
          typename allocator = allocator<std::pair<const key, value>>>
using unordered_map = std::unordered_map<key, value, hasher, traits, allocator>;
}

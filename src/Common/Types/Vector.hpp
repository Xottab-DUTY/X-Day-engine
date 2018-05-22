#pragma once
#include "Allocator.hpp"
#include <vector>

namespace XDay
{
template <typename type, typename allocator = allocator<type>>
using vector = class std::vector<type, allocator>;
}

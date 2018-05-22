#pragma once
#include "Allocator.hpp"
#include <list>

namespace XDay
{
template <typename type, typename allocator = allocator<type>>
using list = std::list<type, allocator>;
}

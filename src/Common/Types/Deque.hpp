#pragma once
#include "Allocator.hpp"
#include <deque>

namespace XDay
{
template <typename type, typename allocator = allocator<type>>
using deque = std::deque<type, allocator>;
}

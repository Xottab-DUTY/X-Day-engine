#pragma once
#include "Allocator.hpp"
#include <map>

namespace XDay
{
template <typename key, class value, class predicate = std::less<key>, typename allocator = allocator<std::pair<const key, value>>>
using map = std::map<key, value, predicate, allocator>;

template <typename key, class value, class predicate = std::less<key>, typename allocator = allocator<std::pair<const key, value>>>
using multimap = std::multimap<key, value, predicate, allocator>;
}

#pragma once
#include "Allocator.hpp"
#include <set>

namespace XDay
{
template <typename key, class predicate = std::less<key>, typename allocator = allocator<key>>
using set = std::set<key, predicate, allocator>;

template <typename key, class predicate = std::less<key>, typename allocator = allocator<key>>
using multiset = std::multiset<key, predicate, allocator>;
}

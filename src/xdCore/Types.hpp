#pragma once

#include <string>
#include <vector>
#include <bitset>

using pstr = char*;
using pcstr = const char*;
using pcstrc = const char* const;

namespace XDay
{
template<typename Type>
using allocator = std::allocator<Type>;

using string = std::basic_string<char, std::char_traits<char>, allocator<char>>;
using stringc = string const;

template<typename Type>
using vector = std::vector<Type, allocator<Type>>;

template<typename FlagsEnum>
using flags = std::bitset<FlagsEnum::LastFlag>;
} // namespace XDay

#pragma once

#include <string>
#include <vector>
#include <bitset>

/*!
*   \typedef using cbool = const bool;
*   \brief constant boolean
*   
*   \typedef using pstr = char*
*   \brief pointer to string
*   
*   \typedef using cpstr = char* const;
*   \brief const pointer to string
*   
*   \typedef using pcstr = const char*;
*   \brief pointer to const string
*   
*   \typedef using cpcstr = const char* const;
*   \brief const pointer to const string
*   
*   \typedef using pstr = char*
*   \brief pointer to string
*/

using cbool = const bool;

using pstr = char*;
using cpstr = char* const;
using pcstr = const char*;
using cpcstr = const char* const;

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

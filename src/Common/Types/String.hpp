#pragma once
#include "Allocator.hpp"
#include <string>

namespace XDay
{
using string = std::basic_string<char, std::char_traits<char>, allocator<char>>;
using stringc = string const;

using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t>>;
using wstringc = wstring const;

using u16string = std::basic_string<char16_t, std::char_traits<char16_t>, allocator<char16_t>>;
using u16stringc = u16string const;

using u32string = std::basic_string<char32_t, std::char_traits<char32_t>, allocator<char32_t>>;
using u32stringc = u32string const;
}

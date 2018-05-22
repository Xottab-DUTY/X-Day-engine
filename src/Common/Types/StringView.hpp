#pragma once
#include <string_view>

namespace XDay
{
using string_view = std::basic_string_view<char>;
using wstring_view = std::basic_string_view<wchar_t>;
using u16string_view = std::basic_string_view<char16_t>;
using u32string_view = std::basic_string_view<char32_t>;
}

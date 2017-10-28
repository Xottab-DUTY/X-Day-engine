#pragma once

#include <string>
#include <vector>
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include <tbb/cache_aligned_allocator.h>

using pstr = char*;
using pcstr = const char*;

namespace XDay
{
template<class T>
class allocator : public tbb::cache_aligned_allocator<T> {};

using string = std::basic_string<char, std::char_traits<char>, allocator<char>>;
using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t>>;
using u16string = std::basic_string<char16_t, std::char_traits<char16_t>, allocator<char16_t>>;
using u32string = std::basic_string<char32_t, std::char_traits<char32_t>, allocator<char32_t>>;

using stringbuf = std::basic_stringbuf<char, std::char_traits<char>, allocator<char>>;
using istringstream = std::basic_istringstream<char, std::char_traits<char>, allocator<char>>;
using ostringstream = std::basic_ostringstream<char, std::char_traits<char>, allocator<char>>;
using stringstream = std::basic_stringstream<char, std::char_traits<char>, allocator<char>>;

template<class T>
class vector : public std::vector<T, allocator<T>> {};
} // namespace XDay

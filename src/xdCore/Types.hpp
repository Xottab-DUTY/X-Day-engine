#pragma once

#include <string>
#include <vector>
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

//#include <tbb/cache_aligned_allocator.h>

using pstr = char*;
using pcstr = const char*;

namespace XDay
{
template<class T>
//class allocator : public tbb::cache_aligned_allocator<T> {};
class allocator : public std::allocator<T> {};

template<class T>
class vector : public std::vector<T, allocator<T>> {};
} // namespace XDay

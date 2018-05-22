#pragma once
#include "Vector.hpp"
#include <stack>

namespace XDay
{
template <typename type, class container = vector<type>>
using stack = std::stack<type, container>;
}

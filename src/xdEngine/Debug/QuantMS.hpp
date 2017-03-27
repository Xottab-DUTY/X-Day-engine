// thanks to hoggy on http://www.cyberforum.ru/cpp-beginners/thread1533454.html
#pragma once

#include <cstdint>

using MicroSeconds = uint64_t;

/*
It will return interval in microseconds between the last call of this method

How to use:
#include <fmt/format.h> // Don't forget to include this header if it's not included
QuantMS();
something
auto timer = QuantMS();
Console->Log(fmt::format("Time elapsed {}", timer)); // Don't use this when Console is not initialized
*/
MicroSeconds QuantMS();

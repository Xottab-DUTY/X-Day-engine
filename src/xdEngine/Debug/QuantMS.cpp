// thanks to hoggy on http://www.cyberforum.ru/cpp-beginners/thread1533454.html
#include <cstdint>
#include <chrono>

#include "QuantMS.hpp"

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
MicroSeconds QuantMS()
{
    using MSec = std::chrono::microseconds;
    using Clock = std::chrono::high_resolution_clock;

    static Clock::time_point last;

    const Clock::time_point now = Clock::now();

    const auto result = std::chrono::duration_cast<MSec>(now - last);

    last = now;

    return static_cast<MicroSeconds>(result.count());
}

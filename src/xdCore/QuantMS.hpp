// thanks to hoggy on http://www.cyberforum.ru/cpp-beginners/thread1533454.html
#pragma once
#ifndef QuantMS_hpp__
#define QuantMS_hpp__

#include <cstdint>

using MicroSeconds = uint64_t;
/*
It will return interval in microseconds between the last call of this method

How to use:
#include <fmt/format.h> // Don't forget to include this header if it's not included
QuantMS();
{
    //something
}
QuantMS_Timer = QuantMS();
Console->Log(fmt::format("Time elapsed {}", QuantMS_Timer)); // Don't use this when Console is not initialized
*/
MicroSeconds QuantMS();

#endif // QuantMS_h__
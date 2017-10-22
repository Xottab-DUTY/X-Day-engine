#pragma once

#include <string>

#include <fmt/format.h>

#include "Common/import_export_macros.hpp"

#include "ConsoleCommand.hpp"

namespace XDay
{
namespace Command
{
template <typename T>
class XDENGINE_API Value : public command
{
public:
    Value(std::string _name, std::string _description, T& _value, T const _min, T const _max,
          bool _save_allowed = true, bool _enabled = true)
        : command(_name, _description, _enabled, true, false, _save_allowed), value(_value), min(_min), max(_max)
    {
        static_assert(!std::is_unsigned_v<T> && std::is_arithmetic_v<T>);
    }

    void Execute() override {}

    void Execute(const std::string& args) override
    {
        if (typeid(value) == typeid(int) /*|| typeid(value) == typeid(unsigned int)*/)
            Assign(stoi(args));

        else if (typeid(value) == typeid(long))
            Assign(stol(args));

        else if (typeid(value) == typeid(unsigned long))
            Assign(stoul(args));

        else if (typeid(value) == typeid(long long))
            Assign(stoll(args));

        else if (typeid(value) == typeid(unsigned long long))
            Assign(stoull(args));

        else if (typeid(value) == typeid(float))
            Assign(stof(args));

        else if (typeid(value) == typeid(double))
            Assign(stod(args));

        else if (typeid(value) == typeid(long double))
            Assign(stold(args));

        else
            InvalidSyntax(args);
    }

    std::string Info() const override
    {
        return fmt::format("{} value", typeid(T).name());
    }

    std::string Status() const override
    {
        return std::to_string(value);
    }

    std::string Syntax() const override
    {
        return fmt::format("range [{}, {}]", min, max);
    }

private:
    void Assign(const T& _value)
    {
        if (_value < min || _value > max)
            InvalidSyntax(std::to_string(_value));
        else
            value = _value;
    }

protected:
    T& value;
    T min;
    T max;
};
} // namespace Command
} // namespace XDay

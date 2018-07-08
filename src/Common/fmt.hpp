#pragma once

#include "Common/Types/String.hpp"

#include <fmt/format.h>

namespace fmt
{
template <>
struct formatter<XDay::string>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const XDay::string& to_format, FormatContext& ctx)
    {
        return format_to(ctx.begin(), to_format);
    }
};
}
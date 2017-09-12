#pragma once
#ifndef Console_hpp__
#define Console_hpp__

#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include "Common/import_export_macros.hpp"

namespace XDay
{
class XDAY_API xdConsole
{
public:
    bool isVisible = false;
    filesystem::path ConfigFile;

    void Initialize();

    void Show();
    void Hide();
};
}

extern XDAY_API XDay::xdConsole* Console;

#endif // Console_hpp__

#pragma once

#include "Key.hpp"
namespace XDay::CommandLine
{
constexpr cpcstr KeyPrefix = "--p_";

static Key KeyDebug("debug", "Enables debug mode", KeyType::Boolean);
static Key KeyNoLog("nolog", "Disables engine log", KeyType::Boolean);
static Key KeyNoLogFlush("nologflush", "Disables log flushing", KeyType::Boolean);
static Key KeyDontHideSystemConsole("syscmd", "Disables system console hiding", KeyType::Boolean);

static Key KeyResPath("respath", "Specifies path of resources folder, default is \"*WorkingDirectory*/resources\"", KeyType::String);
static Key KeyDataPath("datapath", "Specifies path of application data folder, default is \"*WorkingDirectory*/appdata\"", KeyType::String);
static Key KeyMainConfig("mainconfig", "Specifies path and name of main config file (path/name.extension), default is \"*DataPath*/main.config\"", KeyType::String);

static Key KeyName("name", "Specifies AppName, default is \"X-Day Engine\"", KeyType::String);
} // XDay::CommandLine

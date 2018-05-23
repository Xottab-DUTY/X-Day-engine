#pragma once

#include "Key.hpp"
/*!
*   \defgroup command_line CommandLine
*   \ingroup core
*
*   \brief Command line keys management
*   Depends on Core command line management
*
*   \author Xottab_DUTY
*/

/*!
*   \namespace CommandLine
*   \ingroup command_line
*/
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

/*!
* \class Keys
* \ingroup command_line
*
* \brief Singleton that holds all known keys
*/
class XDCORE_API Keys
{
    vector<std::shared_ptr<KeyData>> keys;
    bool autoInitAllowed = false;

public:
    ~Keys();

    static Keys* singleton;
    inline static Keys* Instance();

    static void Initialize() noexcept;
    static void Destroy();

    static std::shared_ptr<KeyData> AddKey(stringc _name, stringc _description, const KeyType _type) noexcept;
    static std::shared_ptr<KeyData> GetKey(stringc _name) noexcept;

    static bool isAutoInitAllowed() noexcept;

    static void Help() noexcept;
};
} // XDay::CommandLine

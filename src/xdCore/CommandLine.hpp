#pragma once

/**
*   \defgroup command_line CommandLine
*   \ingroup core
*
*   \brief Command line keys management
*   Depends on Core command line management
*
*   \author Xottab_DUTY
*/

/**
*   \namespace CommandLine
*   \ingroup command_line
*/
namespace XDay::CommandLine
{
constexpr cpcstr KeyPrefix = "--p_";

class Key;
extern XDCORE_API Key KeyDontHideSystemConsole;

/**
* \enum KeyType
* \ingroup command_line
*/
enum class KeyType
{
    Boolean,
    Number,
    String
};

/**
* \class Key
*
* \ingroup command_line
*
* \brief Key information holder
*/
class XDCORE_API Key
{
    KeyType type;
    cpcstr name;
    pcstr description;
    string stringValue;
    float floatValue;
    bool isSet;
    bool isInitialized;

public:
    Key(cpcstr name, cpcstr description, const KeyType type);

    void Initialize() noexcept;

    void Localize(cpcstr localization) noexcept
    {
        description = localization;
    }

    stringc Name()        const { return name; }
    stringc Description() const { return description; }
    stringc StringValue() const { return stringValue; }
    auto    FloatValue()  const { return floatValue; }
    auto    Type()        const { return type; }
    auto    IsSet()       const { return isSet; }
};

/**
* \class Keys
* \ingroup command_line
*
* \brief Singleton that holds all known keys
*/
class XDCORE_API Keys
{
    static Keys instance;
    vector<Key*> keys;
    bool autoInitAllowed = false;

public:
    static void Initialize() noexcept;
    static void Destroy() noexcept;

    static bool AddKey(Key* newKey) noexcept;
    static Key* GetKey(cpcstr keyName) noexcept;

    static bool IsAutoInitAllowed() noexcept;

    static void Help() noexcept;
};
} // XDay::CommandLine

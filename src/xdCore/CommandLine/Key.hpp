#pragma once

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
/*!
* \enum KeyType
* \ingroup command_line
*/
enum class KeyType
{
    Boolean,
    Number,
    String
};

struct KeyData;
class Key;

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

/*!
 * \struct KeyData
 *
 * \ingroup command_line
 *
 * \brief The actual information about key
 */
struct XDCORE_API KeyData
{
    KeyData(stringc _name, stringc _description, const KeyType _type);

    void Initialize() noexcept;

    KeyType type;
    string name;
    string description;
    string localized_description;
    string string_value;
    float float_value;
    bool is_set;
    bool is_initialized;
};

/*!
* \class KeyData
*
* \ingroup command_line
*
* \brief Key information holder 
*/
class XDCORE_API Key
{
    std::shared_ptr<KeyData> data;
    bool is_initialized;

public:
    Key(stringc _name, stringc _description, const KeyType _type);
    Key(stringc _name);
    explicit Key(std::shared_ptr<KeyData> _data);

    void Initialize() noexcept;
    
    void Localize(stringc localization)
    {
        data->localized_description = localization;
    }

    auto Help()          const { return data->localized_description.empty() ? data->description : data->localized_description; }
    auto Name()          const { return data->name; }
    auto Description()   const { return data->description; }
    auto StringValue()   const { return data->string_value; }
    auto FloatValue()    const { return data->float_value; }
    auto Type()          const { return data->type; }
    auto IsSet()         const { return data->is_set; }
};
} // namespace XDay::CommandLine

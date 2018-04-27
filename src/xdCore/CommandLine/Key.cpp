#include "pch.hpp"
#include "Key.hpp"
#include "Keys.hpp"
#include "Core.hpp"

namespace XDay::CommandLine
{
Keys* Keys::singleton = nullptr;
Keys::~Keys()
{
    keys.clear();
}

void Keys::Destroy()
{
    delete singleton;
    singleton = nullptr;
}

inline Keys* Keys::Instance()
{
    if (singleton == nullptr)
        singleton = new Keys;
    return singleton;
}

void Keys::Initialize() noexcept
{
    for (auto& key : Instance()->keys)
        key->Initialize();

    Instance()->autoInitAllowed = true;
}

std::shared_ptr<KeyData> Keys::AddKey(stringc _name, stringc _description, const KeyType _type) noexcept
{
    auto& instance = *Instance();
    for (auto& key : instance.keys)
    {
        if (key->name == _name)
            return key;
    }

    auto ptr = std::make_shared<KeyData>(KeyData(_name, _description, _type));
    instance.keys.emplace_back(ptr);
    return ptr;
}

std::shared_ptr<KeyData> Keys::GetKey(stringc _name) noexcept
{
    auto& instance = *Instance();
    for (auto& key : instance.keys)
    {
        if (key->name == _name)
            return key;
    }
    
    return nullptr;
}

bool Keys::isAutoInitAllowed() noexcept
{
    return Instance()->autoInitAllowed;
}

void Keys::Help() noexcept
{
    Log::Info("Available command line parameters:");
    for (auto& key : Instance()->keys)
    {
        Log::Info("{} – {}", key->name,
            key->localized_description.empty()
            ? key->description
            : key->localized_description);

    }

    Log::Info("Доступные параметры:\n"\
        "--p_name - Задаёт AppName, по умолчанию: \"X-Day Engine\" \n"\
        "--p_game - Задаёт игровую библиотеку для подключения, по умолчанию: \"xdGame\";\n"
        "--p_datapath - Задаёт путь до папки с настройками, по умолчанию: \"*WorkingDirectory*/appdata\"\n"\
        "--p_respath - Задаёт путь до папки с ресурсами, по умолчанию: \"*WorkingDirectory*/resources\"\n"\
        "--p_mainconfig - Задаёт путь и имя главного файла настроек (путь/имя.расширение), по умолчанию: \"*DataPath*/main.config\" \n"\
        "--p_nolog - Полностью выключает лог движка. Может повысить производительность\n"\
        "--p_nologflush - Выключает сброс лога в файл. Не имеет смысла если задан -nolog\n"\
        "--p_debug - Включает режим отладки\n"
        "--p_syscmd - Отключает скрытие системной консоли\n"
        "--p_shrec - Сборка шейдеров даже если они уже собраны\n"\
        "--p_shpre - Сохраняет обработанные шейдеры в папку исходников шейдеров. Работает только в режиме отладки\n"\
        "--p_texture - Задаёт путь до текстуры для загрузки, по умолчанию: \"texture.dds\"\n"\
        "--p_model - Задаёт путь до модели для загрузки, по умолчанию: \"model.dds\"\n");
}

KeyData::KeyData(stringc _name, stringc _description, const KeyType _type)
    : type(_type), name(_name), description(_description), float_value(0.f),
    is_set(false), is_initialized(false) {}

void KeyData::Initialize() noexcept
{
    if (is_initialized)
        return;

    is_set = Core.FindParam(name);
    if (is_set && type != KeyType::Boolean)
    {
        string_value = Core.ReturnParam(name);

        if (type == KeyType::Number)
            float_value = std::stof(string_value);
    }

    is_initialized = true;
}

Key::Key(stringc _name, stringc _description, const KeyType _type)
    : is_initialized(false)
{
    data = Keys::AddKey(_name, _description, _type);

    is_initialized = data->is_initialized;
    if (!is_initialized && Keys::isAutoInitAllowed())
        Initialize();
}

Key::Key(const std::shared_ptr<KeyData> _data) : data(_data), is_initialized(true) {}

Key::Key(stringc _name)
{
    data = Keys::GetKey(_name);

    if (data)
    {
        is_initialized = true;
        return;
    }

    is_initialized = false;
    Log::Error("Command line key not found: {}", _name);

    // XXX: Add crash here
}

void Key::Initialize() noexcept
{
    if (is_initialized)
        return;

    data->Initialize();
}
}
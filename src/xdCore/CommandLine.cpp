#include "pch.hpp"
#include "CommandLine.hpp"
#include "Core.hpp"
#include "Debug.hpp"

namespace XDay::CommandLine
{
#pragma region Key
Key::Key(cpcstr name, cpcstr description, const KeyType type)
    : type(type), name(name), description(description), isInitialized(false)
{
    ASSERT(Keys::AddKey(this), "{}:: trying to add keys that already exist", __FUNCTION__);

    if (!isInitialized && Keys::IsAutoInitAllowed())
        Initialize();
}

void Key::Initialize() noexcept
{
    if (isInitialized)
        return;

    isSet = Core.FindParam(name);
    if (isSet && type != KeyType::Boolean)
    {
        stringValue = Core.ReturnParam(name);

        if (type == KeyType::Number)
            floatValue = std::stof(stringValue);
    }

    isInitialized = true;
}
#pragma endregion Key

#pragma region Keys
Keys Keys::instance;

void Keys::Initialize() noexcept
{
    for (auto& key : instance.keys)
        key->Initialize();

    Localize();
    instance.autoInitAllowed = true;
}

void Keys::Destroy() noexcept
{
    instance.keys.clear();
}

void LocalizeRussian()
{
    auto LocalizeKey = [](cpcstr keyName, cpcstr keyDescription)
    {
        Key* key = Keys::GetKey(keyName);

        if (key)
            key->Localize(keyDescription);
    };

    LocalizeKey("game", "Задаёт игровую библиотеку для подключения, по умолчанию: \"xdGame\"");
    LocalizeKey("datapath", "Задаёт путь до папки с настройками, по умолчанию: \"*WorkingDirectory*/appdata\"");
    LocalizeKey("respath", "Задаёт путь до папки с ресурсами, по умолчанию: \"*WorkingDirectory*/resources\"");
    LocalizeKey("mainconfig", "Задаёт путь и имя главного файла настроек (путь/имя.расширение), по умолчанию: \"*DataPath*/main.config\"");
    LocalizeKey("nolog", "Полностью выключает лог движка. Может повысить производительность.");
    LocalizeKey("nologflush", "Выключает сброс лога в файл. Не имеет смысла если задан -nolog.");
    LocalizeKey("debug", "Включает режим отладки.");
    LocalizeKey("syscmd", "Отключает скрытие системной консоли.");
    LocalizeKey("shrec", "Сборка шейдеров даже если они уже собраны.");
    LocalizeKey("shpre", "Сохраняет обработанные шейдеры в папку исходников шейдеров. Работает только в режиме отладки.");
    LocalizeKey("texture", "Задаёт путь до текстуры для загрузки, по умолчанию: \"texture.dds\"");
    LocalizeKey("model", "Задаёт путь до модели для загрузки, по умолчанию: \"model.dds\"");
}

void Keys::Localize() noexcept
{
    if (false)
        LocalizeRussian();
}

bool Keys::AddKey(Key* newKey) noexcept
{
    for (auto& key : instance.keys)
    {
        if (key->Name() == newKey->Name())
        {
            Log::Error("{}:: trying to add keys that already exist", __FUNCTION__);
            return false;
        }
    }

    instance.keys.push_back(newKey);
    return true;
}

Key* Keys::GetKey(cpcstr keyName) noexcept
{
    for (auto& key : instance.keys)
        if (key->Name() == keyName)
            return key;

    return nullptr;
}

bool Keys::IsAutoInitAllowed() noexcept
{
    return instance.autoInitAllowed;
}

void Keys::Help() noexcept
{
    Log::Info("Available command line parameters:");
    for (const auto& key : instance.keys)
        Log::Info("{} – {}", key->Name(), key->Description());
}
#pragma endregion Keys
} // XDay::CommandLine

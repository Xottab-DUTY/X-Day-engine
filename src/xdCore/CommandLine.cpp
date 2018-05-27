#include "pch.hpp"
#include "CommandLine.hpp"
#include "Core.hpp"
#include "Debug.hpp"

namespace XDay::CommandLine
{
XDCORE_API Key KeyDontHideSystemConsole("syscmd", "Disables system console hiding", KeyType::Boolean);

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

    instance.autoInitAllowed = true;
}

void Keys::Destroy() noexcept
{
    instance.keys.clear();
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
#pragma endregion Keys
} // XDay::CommandLine

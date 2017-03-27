// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include <GLFW/glfw3.h>

#include "xdCore.hpp"
#include "Console.hpp"
#include "xdEngine.hpp"

void InitializeConsole()
{
    Console = new xdConsole;
    Console->Initialize();
}

void destroyConsole()
{
    Console->CloseLog();
    delete Console;
}

void HelpCmdArgs()
{
    Console->Log("\nUse parameters with values only with commas(-param \"value\")\n"\
        "-param value will return \"alue\"\n"\
        "-param \"value\" will return \"value\"\n\n"\
        "Available parameters:\n"\
        "-name - Specifies AppName, default is \"X-Day Engine\" \n"\
        "-game - Specifies game library to be attached, default is \"xdGame\";\n"
        "-datapath - Specifies path of application data folder, default is \"*WorkingDirectory*/appdata\"\n"\
        "-respath - Specifies path of resources folder, default is \"*WorkingDirectory*/res\"\n"\
        "-mainconfig - Specifies path and name of main config file (path/name.extension), default is \"*DataPath*/main.config\" \n"\
        "-mainlog - Specifies path and name of main log file (path/name.extension), default is \"*DataPath*/main.log\"\n"\
        "-nolog - Completely disables engine log. May increase performance\n"\
        "-nologflush - Disables log flushing. Useless if -nolog defined");

    Console->Log("\nИспользуйте параметры только с кавычками(-параметр \"значение\")\n"\
        "-параметр значение вернёт \"начени\"\n"\
        "-параметр \"значение\" вернёт \"значение\"\n"\
        "\nДоступные параметры:\n"\
        "-name - Задаёт AppName, по умолчанию: \"X-Day Engine\" \n"\
        "-game - Задаёт игровую библиотеку для подключения, по умолчанию: \"xdGame\";\n"
        "-datapath - Задаёт путь до папки с настройками, по умолчанию: \"*WorkingDirectory*/appdata\"\n"\
        "-respath - Задаёт путь до папки с ресурсами, по умолчанию: \"*WorkingDirectory*/res\"\n"\
        "-mainconfig - Задаёт путь и имя главного файла настроек (путь/имя.расширение), по умолчанию: \"*DataPath*/main.config\" \n"\
        "-mainlog - Задаёт путь и имя главного лог файла (путь/имя.расширение), по умолчанию: \"*DataPath*/main.log\"\n"\
        "-nolog - Полностью выключает лог движка. Может повысить производительность\n"\
        "-nologflush - Выключает сброс лога в файл. Не имеет смысла если задан -nolog", false);
}

void Startup()
{
    
}

int main(int argc, char* argv[])
{
#ifdef WINDOWS
    system("chcp 65001");
#endif
    Core.Initialize("X-Day Engine", **argv);
    InitializeConsole();

    Console->Log(Core.GetGLFWVersionString());
    Console->Log(Core.GetBuildString());
    Console->Log("Core.Params: " + Core.Params);
    Console->Log("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.", false);
    Console->Log("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");

    HelpCmdArgs();

    Startup();

    destroyConsole();

    system("pause");
    return 0;
}
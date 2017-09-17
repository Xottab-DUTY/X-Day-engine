// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include <iostream>
#include <thread>

#include <GLFW/glfw3.h>

#include "Common/Platform.hpp"
#include "Debug/Log.hpp"
#include "xdCore.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleCommands.hpp"
#include "xdEngine.hpp"
#include "Renderer/VkDemoRenderer.hpp"

void InitializeConsole()
{
    ConsoleCommands = new XDay::CC_Container;
    Console = new XDay::xdConsole;
    Console->Initialize();
}

void destroyConsole()
{
    ConsoleCommands->Execute(&ConfigSaveCC);
    ConsoleCommands->Execute(&FlushLogCC);
    ConsoleCommands->Destroy();
    delete ConsoleCommands;
    delete Console;
}

void HelpCmdArgs()
{
    Info("\nAvailable parameters:\n"\
        "--p_name - Specifies AppName, default is \"X-Day Engine\" \n"\
        "--p_game - Specifies game module to be attached, default is \"xdGame\";\n"\
        "--p_datapath - Specifies path of application data folder, default is \"*WorkingDirectory*/appdata\"\n"\
        "--p_respath - Specifies path of resources folder, default is \"*WorkingDirectory*/resources\"\n"\
        "--p_mainconfig - Specifies path and name of main config file (path/name.extension), default is \"*DataPath*/main.config\" \n"\
        "--p_nolog - Completely disables engine log. May increase performance\n"\
        "--p_nologflush - Disables log flushing. Useless if -nolog defined\n"\
        "--p_debug - Enables debug mode\n"
        "--p_syscmd - Disables system console hiding\n"
        "--p_shrec - Compile shaders even if they already compiled\n"\
        "--p_shpre - Outputs preprocessed shaders to the shader sources dir. Works only in debug mode\n"\
        "--p_texture - Specifies path to texture file to load, default is \"texture.dds\"\n"\
        "--p_model - Specifies path to model file to model, default is \"model.dds\"\n");

    Info("\nДоступные параметры:\n"\
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

void threadedConsole()
{
    while (!glfwWindowShouldClose(Engine.window))
    {
        std::string input;
        std::getline(std::cin, input);
        if (Console && ConsoleCommands && !glfwWindowShouldClose(Engine.window))
            ConsoleCommands->Execute(input);
        else
            break;
    }
}

void Startup()
{
    while (!glfwWindowShouldClose(Engine.window))
    {
        glfwPollEvents();

        while (VkDemoRender.renderPaused == true)
        {
            glfwWaitEvents();
        }

        VkDemoRender.UpdateUniformBuffer();
        VkDemoRender.DrawFrame();
    }
    VkDemoRender.device->waitIdle();
}

int main(int argc, char* argv[])
{
    system("chcp 65001");

    Core.InitializeArguments(argc, argv);

#ifdef WINDOWS
    if (!Core.FindParam(XDay::eParamDontHideSystemConsole))
        FreeConsole();
#endif

    Core.Initialize();
    InitializeConsole();

    DebugMsg(Core.GetGLFWVersionString());
    Info(Core.GetBuildString());
    Info("Core.Params: " + Core.ParamsString);
    Info("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.");
    Info("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    HelpCmdArgs();

    ConsoleCommands->ExecuteConfig(Console->ConfigFile);
    ErrorIf(!glfwInit(), "GLFW not initialized.");

    Engine.Initialize();
    Engine.createMainWindow();

    std::thread WatchConsole(threadedConsole);
    if (Core.FindParam(XDay::eParamDontHideSystemConsole))
        WatchConsole.detach();

    VkDemoRender.Initialize();

    Startup();

    WatchConsole.~thread();

    glfwTerminate();

    destroyConsole();

    return 0;
}

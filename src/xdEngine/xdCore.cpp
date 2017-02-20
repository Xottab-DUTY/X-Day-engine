#include <iostream>
#include <ctime>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "xdCore.hpp"
#include "Console.hpp"

XDENGINE_API xdCore Core;

void xdCore::Initialize(std::string&& _AppName, char& argv)
{
    FindParam("-name") ? AppName = ReturnParam("-name") : AppName = _AppName;
    FindParam("-game") ? GameModule = ReturnParam("-game") : GameModule = "xdGame";
    Params = &argv;
    AppPath = filesystem::absolute(&Params[0]);
    WorkPath = filesystem::current_path();
    binPath = WorkPath.string() + "/bin/";
    FindParam("-datapath") ? DataPath = ReturnParam("-datapath") : DataPath = WorkPath.string() + "/appdata/"; CreateDirIfNotExist(DataPath.c_str());
    FindParam("-respath") ? ResPath = ReturnParam("-respath") : ResPath = WorkPath.string() + "/res/"; CreateDirIfNotExist(ResPath.c_str());

    configsPath = ResPath.string() + "/configs/"; CreateDirIfNotExist(configsPath.c_str());
    texturesPath = ResPath.string() + "/textures/"; CreateDirIfNotExist(texturesPath.c_str());
    modelsPath = ResPath.string() + "/models/"; CreateDirIfNotExist(modelsPath.c_str());
    soundsPath = ResPath.string() + "/sounds/"; CreateDirIfNotExist(soundsPath.c_str());
    archivesPath = ResPath.string() + "/archives/"; CreateDirIfNotExist(archivesPath.c_str());

    CalculateBuildId();

    buildString << "xdCore build " << buildId << ", " << buildDate << ", " << buildTime;
}

// Finds command line parameters and returns true if param exists
bool xdCore::FindParam(std::string&& Param)
{
    if (Params.find(Param) != std::string::npos)
        return true;
    return false;
}

// Finds command line parameter and returns it's value.
// Keep in mind that command line parameter:
// -name value will return "alue"
// -name "value" will return correct "value"
// Do not use ReturnParam() if FindParam() returns false
// else you will get an unexpected behavior
std::string xdCore::ReturnParam(std::string&& Param)
{
    Param = Params.substr(Params.find(Param) + (Param.length()+2), Params.find(Param+" %[^ ]"));
    Param.pop_back(); // Costyl (TM)
    return Param;
}

void xdCore::CreateDirIfNotExist(filesystem::path&& p)
{
    if (!filesystem::exists(p)) filesystem::create_directory(p);
}

void xdCore::CalculateBuildId()
{
    // All started in ~01.01.2017
    std::tm startDate;
    { // Start date and time
        startDate.tm_mday = 1;
        startDate.tm_mon = 0;
        startDate.tm_year = 2017 - 1900;
        startDate.tm_hour = 12; //random hour(don't remember exact hour)
        startDate.tm_min = 0;
        startDate.tm_sec = 0;
        
    }

    std::tm buildDate_tm;
    { // Build date
        std::string stringMonth;
        std::istringstream buffer(buildDate);
        buffer >> stringMonth >> buildDate_tm.tm_mday >> buildDate_tm.tm_year;
        buildDate_tm.tm_year -= 1900;

        const char* monthId[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        for (int i = 0; i < 12; i++)
        {
            if (stringMonth.compare(monthId[i])) continue;
            buildDate_tm.tm_mon = i;
            break;
        }
    }
    { // Build time
        std::string timeBuffer(buildTime);
        std::replace(timeBuffer.begin(), timeBuffer.end(), ':', ' '); // Costyl (TM)
        std::istringstream buffer2(timeBuffer);
        buffer2 >> buildDate_tm.tm_hour >> buildDate_tm.tm_min >> buildDate_tm.tm_sec;
    }
    
    buildId = -difftime(std::mktime(&startDate), std::mktime(&buildDate_tm)) / 86400;
}
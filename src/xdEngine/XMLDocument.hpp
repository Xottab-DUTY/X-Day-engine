#pragma once
#ifndef XMLDocument_hpp__
#define XMLDocument_hpp__
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;


#include "tinyxml2/tinyxml2.h"

#include "xdEngine_impexp.inl"
#include "xdCore.hpp"

class XDAY_API xdXMLDocument
{
public:
    xdXMLDocument();
    ~xdXMLDocument();

    void Load(filesystem::path&& resources_type, filesystem::path&& path, std::string&& xml_filename);
    void Load(filesystem::path&& full_path);

    //used for resources_type
    const filesystem::path& AppData = Core.DataPath;
    const filesystem::path& Resources = Core.ResourcesPath;
    const filesystem::path& Archives = Core.ArchivesPath;
    const filesystem::path& Configs = Core.ConfigsPath;
    const filesystem::path& Models = Core.ModelsPath;
    const filesystem::path& Sounds = Core.SoundsPath;
    const filesystem::path& Textures = Core.TexturesPath;

protected:
    tinyxml2::XMLNode* root_node;

private:
    tinyxml2::XMLDocument xml_document;
};
#endif // !XMLDocument_hpp__

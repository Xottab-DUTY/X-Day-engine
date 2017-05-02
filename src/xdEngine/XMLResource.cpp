#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "tinyxml2/tinyxml2.h"

#include "XMLResource.hpp"
#include "Console.hpp"

xdXMLResource::xdXMLResource()
    : super() {}

xdXMLResource::xdXMLResource(filesystem::path resources_type, filesystem::path _path, std::string xml_filename)
    : super(resources_type, _path, xml_filename) {}

xdXMLResource::xdXMLResource(filesystem::path resources_type, std::string xml_filename)
    : super(resources_type, xml_filename) {}

xdXMLResource::xdXMLResource(filesystem::path full_path)
    : super(full_path) {}

void xdXMLResource::ParseResources()
{
    tinyxml2::XMLElement* Resource = xml_document.FirstChildElement()->FirstChildElement("resource");
    tinyxml2::XMLPrinter printer;
    Resource->Accept(&printer);
    ConsoleMsg("\nElement: {}",printer.CStr());
    const char* Attribute = nullptr;
    Attribute = Resource->Attribute("type");
    if (strcmp(Resource->Attribute("type"), "") == 0)
    {
        Console->Log("xml failed");
    }
    else if (strcmp(Resource->Attribute("type"), "archives") == 0)
    {
        ConsoleMsg("archives: {}", Resource->Attribute("path"));
    }
    else if (strcmp(Resource->Attribute("type"), "configs") == 0)
    {
        ConsoleMsg("configs: {}", Resource->Attribute("path"));
    }
    else if (strcmp(Resource->Attribute("type"), "models") == 0)
    {
        ConsoleMsg("models: {}", Resource->Attribute("path"));
    }
    else if (strcmp(Resource->Attribute("type"), "sounds") == 0)
    {
        ConsoleMsg("sounds: {}", Resource->Attribute("path"));
    }
    else if (strcmp(Resource->Attribute("type"), "textures") == 0)
    {
        ConsoleMsg("textures: {}", Resource->Attribute("path"));
    }
}


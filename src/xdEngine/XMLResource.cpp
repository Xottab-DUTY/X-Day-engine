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
    for (auto resource = root_node->NextSiblingElement("resources")->FirstChildElement("resource");
        resource;
        resource = resource->NextSiblingElement("resource"))
    {
        ConsoleMsg("{}: {}", resource->Attribute("type"), resource->FirstChildElement("path")->GetText());
    }
}


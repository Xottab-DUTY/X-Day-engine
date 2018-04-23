#include "pch.hpp"
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include "tinyxml2/tinyxml2.h"

#include "XMLResource.hpp"
#include "Core.hpp"

using namespace XDay;

xdXMLResource::xdXMLResource()
    : super() {}

xdXMLResource::xdXMLResource(const filesystem::path& resources_type, const filesystem::path& _path, const std::string& xml_filename)
    : super(resources_type, _path, xml_filename) {}

xdXMLResource::xdXMLResource(const filesystem::path& resources_type, const std::string& xml_filename)
    : super(resources_type, xml_filename) {}

xdXMLResource::xdXMLResource(const filesystem::path& full_path)
    : super(full_path) {}

void xdXMLResource::ParseResources() const
{
    for (auto resource = root_node->NextSiblingElement("resources")->FirstChildElement("resource");
        resource;
        resource = resource->NextSiblingElement("resource"))
    {
        if (strcmp(resource->Attribute("type"), "archives") == 0)
            if (!resource->BoolAttribute("absolute"))
                Core.ArchivesPath = Core.ResourcesPath.string() + resource->FirstChildElement("path")->GetText();
            else
                Core.ArchivesPath = resource->FirstChildElement("path")->GetText();

        else if (strcmp(resource->Attribute("type"), "configs") == 0)
            if (!resource->BoolAttribute("absolute"))
                Core.ConfigsPath = Core.ResourcesPath.string() + resource->FirstChildElement("path")->GetText();
            else
                Core.ConfigsPath = resource->FirstChildElement("path")->GetText();

        else if (strcmp(resource->Attribute("type"), "models") == 0)
            if (!resource->BoolAttribute("absolute"))
                Core.ModelsPath = Core.ResourcesPath.string() + resource->FirstChildElement("path")->GetText();
            else
                Core.ModelsPath = resource->FirstChildElement("path")->GetText();

        else if (strcmp(resource->Attribute("type"), "shaders") == 0)
            if (!resource->BoolAttribute("absolute"))
                Core.ShadersPath = Core.ResourcesPath.string() + resource->FirstChildElement("path")->GetText();
            else
                Core.ShadersPath = resource->FirstChildElement("path")->GetText();

        else if (strcmp(resource->Attribute("type"), "sounds") == 0)
            if (!resource->BoolAttribute("absolute"))
                Core.SoundsPath = Core.ResourcesPath.string() + resource->FirstChildElement("path")->GetText();
            else
                Core.SoundsPath = resource->FirstChildElement("path")->GetText();

        else if (strcmp(resource->Attribute("type"), "textures") == 0)
            if (!resource->BoolAttribute("absolute"))
                Core.TexturesPath = Core.ResourcesPath.string() + resource->FirstChildElement("path")->GetText();
            else
                Core.TexturesPath = resource->FirstChildElement("path")->GetText();
    }
}


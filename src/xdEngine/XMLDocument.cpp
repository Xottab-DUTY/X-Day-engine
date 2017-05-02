#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "tinyxml2/tinyxml2.h"

#include "XMLDocument.hpp"
#include "Console.hpp"

xdXMLDocument::xdXMLDocument()
{

}

xdXMLDocument::xdXMLDocument(filesystem::path resources_type, filesystem::path _path, std::string xml_filename)
{
    Load(resources_type, _path, xml_filename);
    root_node = nullptr;
}

xdXMLDocument::xdXMLDocument(filesystem::path resources_type, std::string xml_filename)
{
    Load(resources_type, xml_filename);
    root_node = nullptr;
}

xdXMLDocument::xdXMLDocument(filesystem::path full_path)
{
    Load(full_path);
    root_node = nullptr;
}

xdXMLDocument::~xdXMLDocument()
{

}

void xdXMLDocument::Load(filesystem::path resources_type, filesystem::path _path, std::string xml_filename)
{
    std::string buffer(resources_type.string() + _path.string() + xml_filename);

    if (xml_document.LoadFile(buffer.c_str()) == 0)
        ConsoleMsg("XML file loaded: {}", buffer)
    else
        ConsoleMsg("XML failed to load file: {}", buffer);
    root_node = xml_document.FirstChild();
    xml_document.Print();
}

void xdXMLDocument::Load(filesystem::path resources_type, std::string xml_filename)
{
    std::string buffer(resources_type.string() + xml_filename);

    if (xml_document.LoadFile(buffer.c_str()) == 0)
        ConsoleMsg("XML file loaded: {}", buffer)
    else
        ConsoleMsg("XML failed to load file: {}", buffer);
    root_node = xml_document.FirstChild();
    xml_document.Print();
}

void xdXMLDocument::Load(filesystem::path full_path)
{
    if (xml_document.LoadFile(full_path.string().c_str())) // string().c_str() what a nice costyl
        ConsoleMsg("XML file loaded: {}", full_path.string())
    else
        ConsoleMsg("XML failed to load file: {}", full_path.string());
    root_node = xml_document.FirstChild();
    xml_document.Print();
}

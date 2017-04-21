#include "XMLDocument.hpp"

#include "Console.hpp"

xdXMLDocument::xdXMLDocument()
{

}

xdXMLDocument::~xdXMLDocument()
{

}

void xdXMLDocument::Load(filesystem::path&& resources_type, filesystem::path&& path, std::string&& xml_filename)
{
    std::string buffer(resources_type.string() + path.string() + xml_filename);

    if (xml_document.LoadFile(buffer.c_str()) == 0)
    {
        ConsoleMsg("XML file loaded: {}", buffer);
    }
}
void xdXMLDocument::Load(filesystem::path&& full_path)
{
    if (xml_document.LoadFile(full_path.string().c_str())) // string().c_str() what a beautiful costyl
    {
        ConsoleMsg("XML file loaded: {}");
    }
}

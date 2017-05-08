#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "tinyxml2/tinyxml2.h"

#include "Debug/Log.hpp"
#include "XMLDocument.hpp"

xdXMLDocument::xdXMLDocument()
{
    root_node = nullptr;
}

xdXMLDocument::xdXMLDocument(filesystem::path resources_type, filesystem::path _path, std::string xml_filename)
{
    root_node = nullptr;
    Load(resources_type, _path, xml_filename);
}

xdXMLDocument::xdXMLDocument(filesystem::path resources_type, std::string xml_filename)
{
    root_node = nullptr;
    Load(resources_type, xml_filename);
}

xdXMLDocument::xdXMLDocument(filesystem::path full_path)
{
    root_node = nullptr;
    Load(full_path);
}

void xdXMLDocument::operator=(filesystem::path full_path)
{
    Load(full_path);
}

xdXMLDocument::~xdXMLDocument()
{

}

void xdXMLDocument::Load(filesystem::path resources_type, filesystem::path _path, std::string xml_filename)
{
    std::string buffer(resources_type.string() + _path.string() + xml_filename);

    if (!xml_document.LoadFile(buffer.c_str()) == 0)
        Msg("XML failed to load file: {}", buffer);

    root_node = xml_document.FirstChild();
}

void xdXMLDocument::Load(filesystem::path resources_type, std::string xml_filename)
{
    std::string buffer(resources_type.string() + xml_filename);

    if (!xml_document.LoadFile(buffer.c_str()) == 0)
        Msg("XML failed to load file: {}", buffer);

    root_node = xml_document.FirstChild();
}

void xdXMLDocument::Load(filesystem::path full_path)
{
    if (!xml_document.LoadFile(full_path.string().c_str())) // string().c_str() what a nice costyl
        Msg("XML failed to load file: {}", full_path.string());

    root_node = xml_document.FirstChild();
}

bool xdXMLDocument::isErrored() const
{
    return xml_document.Error();
}

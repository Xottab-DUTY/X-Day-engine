#pragma once
#ifndef XMLDocument_hpp__
#define XMLDocument_hpp__
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "tinyxml2/tinyxml2.h"

#include "xdEngine_impexp.inl"

class XDAY_API xdXMLDocument
{
public:
    xdXMLDocument();
    xdXMLDocument(filesystem::path resources_type, filesystem::path _path, std::string xml_filename);
    xdXMLDocument(filesystem::path resources_type, std::string xml_filename);
    xdXMLDocument(filesystem::path full_path);

    virtual ~xdXMLDocument();

    void Load(filesystem::path resources_type, filesystem::path _path, std::string xml_filename);
    void Load(filesystem::path resources_type, std::string xml_filename);
    void Load(filesystem::path full_path);

protected:
    tinyxml2::XMLNode* root_node;

private:
    tinyxml2::XMLDocument xml_document;
};
#endif // XMLDocument_hpp__

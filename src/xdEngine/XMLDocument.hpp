#pragma once
#ifndef XMLDocument_hpp__
#define XMLDocument_hpp__
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include "tinyxml2/tinyxml2.h"

#include "xdEngine_impexp.inl"

namespace XDay
{
class XDAY_API xdXMLDocument
{
public:
    xdXMLDocument();
    xdXMLDocument(filesystem::path resources_type, filesystem::path _path, std::string xml_filename);
    xdXMLDocument(filesystem::path resources_type, std::string xml_filename);
    xdXMLDocument(filesystem::path full_path);
    void operator=(filesystem::path full_path);

    virtual ~xdXMLDocument();

    void Load(filesystem::path resources_type, filesystem::path _path, std::string xml_filename);
    void Load(filesystem::path resources_type, std::string xml_filename);
    void Load(filesystem::path full_path);

    bool isErrored() const;

protected:
    tinyxml2::XMLNode* root_node;
    tinyxml2::XMLDocument xml_document;
};
}
#endif // XMLDocument_hpp__

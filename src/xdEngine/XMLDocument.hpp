#pragma once
#ifndef XMLDocument_hpp__
#define XMLDocument_hpp__
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include "tinyxml2/tinyxml2.h"

#include "Common/import_export_macros.hpp"

namespace XDay
{
class XDAY_API xdXMLDocument
{
public:
    xdXMLDocument();
    xdXMLDocument(filesystem::path resources_type, filesystem::path _path, std::string xml_filename);
    xdXMLDocument(filesystem::path resources_type, std::string xml_filename);
    xdXMLDocument(filesystem::path full_path);
    void operator=(const filesystem::path& full_path);

    virtual ~xdXMLDocument();

    void Load(const filesystem::path& resources_type, const filesystem::path& _path, const std::string& xml_filename);
    void Load(const filesystem::path& resources_type, const std::string& xml_filename);
    void Load(const filesystem::path& full_path);

    bool isErrored() const;

protected:
    tinyxml2::XMLNode* root_node;
    tinyxml2::XMLDocument xml_document;
};
}
#endif // XMLDocument_hpp__

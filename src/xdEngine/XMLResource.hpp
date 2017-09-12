#pragma once
#ifndef XMLResource_hpp__
#define XMLResource_hpp__
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include "tinyxml2/tinyxml2.h"

#include "Common/import_export_macros.hpp"
#include "XMLDocument.hpp"

namespace XDay
{
class XDAY_API xdXMLResource : public xdXMLDocument
{
    using super = xdXMLDocument;

public:
    xdXMLResource();
    xdXMLResource(const filesystem::path& resources_type, const filesystem::path& _path, const std::string& xml_filename);
    xdXMLResource(const filesystem::path& resources_type, const std::string& xml_filename);
    xdXMLResource(const filesystem::path& full_path);

    void ParseResources() const;
};
}
#endif // XMLResource_hpp__

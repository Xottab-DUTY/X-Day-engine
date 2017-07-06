#pragma once
#ifndef XMLResource_hpp__
#define XMLResource_hpp__
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "tinyxml2/tinyxml2.h"

#include "xdEngine_impexp.inl"
#include "XMLDocument.hpp"

namespace XDay
{
class XDAY_API xdXMLResource : public xdXMLDocument
{
    using super = xdXMLDocument;

public:
    xdXMLResource();
    xdXMLResource(filesystem::path resources_type, filesystem::path _path, std::string xml_filename);
    xdXMLResource(filesystem::path resources_type, std::string xml_filename);
    xdXMLResource(filesystem::path full_path);

    void ParseResources() const;
};
}
#endif // XMLResource_hpp__

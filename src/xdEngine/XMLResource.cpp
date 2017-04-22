#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "tinyxml2/tinyxml2.h"

#include "XMLResource.hpp"

xdXMLResource::xdXMLResource()
    : super() {}

xdXMLResource::xdXMLResource(filesystem::path resources_type, filesystem::path _path, std::string xml_filename)
    : super(resources_type, _path, xml_filename) {}

xdXMLResource::xdXMLResource(filesystem::path resources_type, std::string xml_filename)
    : super(resources_type, xml_filename) {}

xdXMLResource::xdXMLResource(filesystem::path full_path)
    : super(full_path) {}



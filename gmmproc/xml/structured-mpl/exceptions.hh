#ifndef GMMPROC_XML_STRUCTURED_EXCEPTIONS_HH
#define GMMPROC_XML_STRUCTURED_EXCEPTIONS_HH

#include <gmmproc/xml/xml.hh>

#include <stdexcept>
#include <string>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

// invalid attribute exception

class InvalidAttribute
  : public std::invalid_argument
{
public:
  InvalidAttribute (Xml::Attribute const& attr,
                    std::string const& type,
                    std::string const& what);
};

// invalid parser assumption

class AttributesOnly {};
class ChildrenOnly {};
class DataOnly {};
constexpr AttributesOnly Attrs = AttributesOnly{};
constexpr ChildrenOnly Children = ChildrenOnly{};
constexpr DataOnly Data = DataOnly{};

class InvalidParserAssumption
  : public std::logic_error
{
public:
  InvalidParserAssumption (AttributesOnly, Xml::Node const& node, std::string const& attribute);
  InvalidParserAssumption (ChildrenOnly, Xml::Node const& node, std::string const& child);
  InvalidParserAssumption (DataOnly, Xml::Node const& node);
};

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_EXCEPTIONS_HH

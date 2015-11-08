#ifndef MM_XML_EXCEPTIONS_HH
#define MM_XML_EXCEPTIONS_HH

#include <stdexcept>
#include <string>

#include "base/xml.hh"

namespace Mm
{

namespace Xml
{

// invalid attribute exception

class InvalidAttribute
  : public std::invalid_argument
{
public:
  InvalidAttribute (Base::Attribute const& attr,
                    std::string const& type,
                    std::string const& what);
};

// invalid parser assumption

class AttributesOnly {};
class ChildrenOnly {};
constexpr AttributesOnly Attrs = AttributesOnly{};
constexpr ChildrenOnly Children = ChildrenOnly{};

class InvalidParserAssumption
  : public std::logic_error
{
public:
  InvalidParserAssumption (AttributesOnly, Base::Node const& node, std::string const& attribute);
  InvalidParserAssumption (ChildrenOnly, Base::Node const& node, std::string const& child);
};

} // namespace Xml

} // namespace Mm

#endif // MM_XML_EXCEPTIONS_HH

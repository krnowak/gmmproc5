#ifndef MM_XML_STRUCTURED_GENERIC_ATTR_HH
#define MM_XML_STRUCTURED_GENERIC_ATTR_HH

#include <mm/xml/xml.hh>

#include <sstream>
#include <string>

#include <cstdlib>

namespace Mm
{

namespace Xml
{

namespace Structured
{

namespace StructuredDetails
{

// attribute value getters

template <typename Type>
class AttrValue;

template <>
class AttrValue<std::string>
{
public:
  static std::string
  get (Xml::Attribute& attr);
};

template <>
class AttrValue<int>
{
public:
  static int
  get (Xml::Attribute& attr);
};

template <>
class AttrValue<bool>
{
public:
  static bool
  get (Xml::Attribute& attr);
};

// attribute code

template <typename AttrType>
class DefaultValue
{
public:
  static AttrType
  get ()
  {
    return AttrType {};
  }
};

class TrueBoolValue
{
public:
  static bool
  get ()
  {
    return true;
  }
};

} // namespace StructuredDetails

template <typename NameString, typename AttrType, typename DefaultValue = StructuredDetails::DefaultValue<AttrType>>
class GenericAttr
{
public:
  using Name = NameString;
  using Type = AttrType;

  static void
  set (Type& attr, Xml::Node& node)
  {
    auto raw_attr = node.attribute (Name::raw.data ());
    if (raw_attr)
    {
      attr = StructuredDetails::AttrValue<Type>::get (*raw_attr);
      node.remove (*raw_attr);
    }
    else
    {
      attr = DefaultValue::get ();
    }
  }
};

// some standard attribute aliases

template <typename NameString>
using StringAttr = GenericAttr<NameString, std::string>;

template <typename NameString>
using FalseAttr = GenericAttr<NameString, bool>;

template <typename NameString>
using TrueAttr = GenericAttr<NameString, bool, StructuredDetails::TrueBoolValue>;

template <typename NameString>
using IntAttr = GenericAttr<NameString, int>;

} // namespace Structured

} // namespace Xml

} // namespace Mm

#endif // MM_XML_STRUCTURED_GENERIC_ATTR_HH

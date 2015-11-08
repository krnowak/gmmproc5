#ifndef MM_XML_GENERIC_ATTR_HH
#define MM_XML_GENERIC_ATTR_HH

#include <sstream>
#include <string>

#include <cstdlib>

#include "base/xml.hh"

namespace Mm
{

namespace Xml
{

// attribute value getters

template <typename Type>
class AttrValue;

template <>
class AttrValue<std::string>
{
public:
  static std::string
  get (Base::Attribute& attr);
};

template <>
class AttrValue<int>
{
public:
  static int
  get (Base::Attribute& attr);
};

template <>
class AttrValue<bool>
{
public:
  static bool
  get (Base::Attribute& attr);
};

// attribute code

template <typename AttrType>
class GenericAttrDefaultValue
{
public:
  static AttrType
  get ()
  {
    return AttrType {};
  }
};

template <typename NameString, typename AttrType, typename DefaultValue = GenericAttrDefaultValue<AttrType>>
class GenericAttr
{
public:
  using Name = NameString;
  using Type = AttrType;

  static void
  set (Type& attr, Base::Node& node)
  {
    auto raw_attr = node.attribute (Name::raw.data ());
    if (raw_attr)
    {
      attr = AttrValue<Type>::get (*raw_attr);
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

class TrueBoolValue
{
public:
  static bool
  get ()
  {
    return true;
  }
};

template <typename NameString>
using TrueAttr = GenericAttr<NameString, bool, TrueBoolValue>;

template <typename NameString>
using IntAttr = GenericAttr<NameString, int>;

} // namespace Xml

} // namespace Mm

#endif // MM_XML_GENERIC_ATTR_HH

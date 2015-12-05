#include <algorithm>

#include "exceptions.hh"
#include "generic-attr.hh"

namespace Mm
{

namespace Xml
{

namespace XmlDetails
{

// attr value getters

/* static */ std::string
AttrValue<std::string>::get (Base::Attribute& attr)
{
  return attr.value ();
}

/* static */ int
AttrValue<int>::get (Base::Attribute& attr)
{
  try
  {
    std::size_t read {};
    std::string const str {attr.value ()};
    auto value = std::stoi (str, &read);
    if (read != str.size ())
    {
      std::ostringstream oss;

      oss << "Expected a number, but got some non-digits too: " << str;
      throw InvalidAttribute (attr, "int", oss.str ());
    }
    return value;
  }
  catch (InvalidAttribute const& ex)
  {
    throw;
  }
  catch (std::invalid_argument const& ex)
  {
    throw InvalidAttribute (attr, "int", ex.what ());
  }
}

/* static */ bool
AttrValue<bool>::get (Base::Attribute& attr)
{
  std::string const value = attr.value ();

  if (value == "1")
  {
    return true;
  }
  if (value == "0")
  {
    return false;
  }
  std::ostringstream oss;

  oss << "Expected either '0' or '1', got '" << value << "'";
  throw InvalidAttribute (attr, "boolean", oss.str ());
}

} // namespace XmlDetails

} // namespace Xml

} // namespace Mm

#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>

#include "exceptions.hh"

namespace Mm
{

namespace Xml
{

namespace
{

std::string
join (std::vector<std::string> const& v)
{
  if (v.empty ())
  {
    return "";
  }

  std::ostringstream oss;
  std::for_each (std::crbegin (v), std::prev (std::crend (v)),
                 [&oss] (std::string const& str)
                 {
                   oss << str << " -> ";
                 });
  oss << v.front ();

  return oss.str ();
}

std::string get_name(Base::Node const& node)
{
  std::string n = node.name ();

  if (n.empty ())
  {
    if (!node.parent ())
    {
      return "docroot";
    }
    return "!unknown-element-name!";
  }

  return n;
}

std::string
get_backtrace (Base::Node const& node)
{
  std::vector<std::string> v{};

  std::experimental::optional<Base::Node> n = node;
  while (n)
  {
    std::string const node_name = get_name(*n);
    auto const attr_name = n->attribute("name");

    if (attr_name)
    {
      std::ostringstream oss;

      oss << node_name << " (" << attr_name->value () << ")";
      v.push_back (oss.str ());
    }
    else
    {
      v.push_back (node_name);
    }
  }

  return join (v);
}

std::string
get_ia_error (Base::Attribute const& attr,
              std::string const& type,
              std::string const& what)
{
  std::ostringstream oss;
  std::string const backtrace = get_backtrace (attr.parent ());

  oss << "invalid " << type << " attribute '" << attr.name () << "' in " << backtrace << ": " << what;
  return oss.str ();
}

std::string
get_ipa_error (Base::Node const& node,
               std::string const& type,
               std::string const& name)
{
  std::ostringstream oss;

  oss << "unexpected " << type << " ";
  if (!name.empty ())
  {
    oss << "'" << name << "' ";
  }
  oss << "in " << get_backtrace (node);
  return oss.str ();
}

} // unnamed namespace

// invalid attribute exception

InvalidAttribute::InvalidAttribute (Base::Attribute const& attr,
                                    std::string const& type,
                                    std::string const& what)
  : std::invalid_argument {get_ia_error (attr, type, what)}
{}

// invalid parser assumption

InvalidParserAssumption::InvalidParserAssumption (AttributesOnly,
                                                  Base::Node const& node,
                                                  std::string const& attribute)
  : std::logic_error {get_ipa_error (node, "attribute", attribute)}
{}

InvalidParserAssumption::InvalidParserAssumption (ChildrenOnly,
                                                  Base::Node const& node,
                                                  std::string const& child)
  : std::logic_error {get_ipa_error (node, "child", child)}
{}

InvalidParserAssumption::InvalidParserAssumption (DataOnly,
                                                  Base::Node const& node)
  : std::logic_error {get_ipa_error (node, "text data", "")}
{}

} // namespace Xml

} // namespace Mm

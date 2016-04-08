#include "exceptions.hh"
#include "generic-child.hh"

namespace Mm
{

namespace Xml
{

namespace Structured
{

namespace StructuredDetails
{

std::size_t
siblings_count (Xml::Node const& node)
{
  auto r = node.siblings (node.name ());
  auto b = std::cbegin (r);
  auto e = std::cend (r);
  auto c = 0u;

  // not using distance - it returns a signed value
  while (b != e)
  {
    ++c;
    ++b;
  }
  return c;
}

void
ensure_empty (Xml::Node const& node)
{
  if (!node.text ().empty ())
  {
    throw InvalidParserAssumption (Data, node);
  }
  for (auto c : node.children ())
  {
    throw InvalidParserAssumption (Children, node, c.name ());
  }
  for (auto a : node.attributes ())
  {
    throw InvalidParserAssumption (Attrs, node, a.name ());
  }
}

void
remove_one (Xml::Node const& node)
{
  auto parent = node.parent ();
  if (!parent)
  {
    return;
  }
  parent->remove (node);
}

void
remove_siblings (Xml::Node const& node)
{
  auto parent = node.parent ();
  if (!parent)
  {
    return;
  }
  auto r = node.siblings (node.name ());
  for (auto i = r.begin (), e = r.end (); i != e;)
  {
    auto old = i++;
    parent->remove (*old);
  }
}

} // namespace StructuredDetails

} // namespace Structured

} // namespace Xml

} // namespace Mm

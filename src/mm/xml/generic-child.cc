#include "exceptions.hh"
#include "generic-child.hh"

namespace Mm
{

namespace Xml
{

std::size_t
siblings_count (Base::Node const& node)
{
  auto r = node.siblings (node.name ());

  return std::distance (r.begin (), r.end ());
}

void
ensure_empty (Base::Node const&)
{
  /*
  auto attr = node.first_attribute ();
  if (attr)
  {
    throw InvalidParserAssumption (Attrs, node, attr.name ());
  }
  auto child = node.first_child ();
  if (child)
  {
    throw InvalidParserAssumption (Children, node, child.name ());
  }
  */
}

void
remove_one (Base::Node const& node)
{
  auto parent = node.parent ();
  if (!parent)
  {
    return;
  }
  parent->remove (node);
}

void
remove_siblings (Base::Node const& node)
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

} // namespace Xml

} // namespace Mm
